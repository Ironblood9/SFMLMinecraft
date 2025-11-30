#include "InventroyPanel.h"
#include "TileID.h"

InventoryPanel::InventoryPanel(Inventory& inventory, const sf::Vector2u& windowSize)
    : playerInventory(inventory), isVisible(false), columns(10), rows(5), slotSize(46.f), guiWindowSize(windowSize),
      dragging(false), dragSrcIndex(-1), dragTileId(TILE_AIR), dragQuantity(0) {
    // Eðer inventory kapasitesi panelin varsayýlan grid'inden farklýysa,
    // paneli inventory kapasitesine göre ayarla (görsel büyütme -> veriyle eþleþtirme)
    int invSize = static_cast<int>(playerInventory.getItems().size());
    if (invSize > 0) {
        if (columns > invSize) columns = invSize;
        rows = (invSize + columns - 1) / columns;
    }
    background.setFillColor(sf::Color(50, 50, 50, 220));
    background.setOutlineThickness(2.f);
    background.setOutlineColor(sf::Color::White);
    updateLayout(windowSize);
}

void InventoryPanel::updateLayout(const sf::Vector2u& windowSize) {
    float panelWidth = columns * slotSize + 20.f;
    float panelHeight = rows * slotSize + 20.f;
    position = sf::Vector2f(
        (windowSize.x - panelWidth) / 2.f,
        (windowSize.y - panelHeight) / 2.f
    );
    background.setPosition(position);
    background.setSize(sf::Vector2f(panelWidth, panelHeight));
}

bool InventoryPanel::loadTexture(const std::string& texturePath, const sf::Vector2u& tileSize) {
    if (!texture.loadFromFile(texturePath)) return false;
    this->tileSize = tileSize;

    playerInventory.updateSprites(texture, this->tileSize);
    return true;
}

void InventoryPanel::populateWithTiles(const std::vector<int>& tileIds) {
    for (int id : tileIds) {
        if (!playerInventory.addItem(id, 1)) {
            break;
        }
    }
    if (texture.getSize().x > 0 && tileSize.x > 0) {
        playerInventory.updateSprites(texture, tileSize);
    }
}

void InventoryPanel::toggle() {
    isVisible = !isVisible;
}

void InventoryPanel::draw(sf::RenderWindow& window) {
    if (!isVisible) return;

    window.draw(background);

    static sf::Font font;
    static bool fontLoaded = font.openFromFile("assets/font.ttf");

    if (fontLoaded) {
        sf::Text title(font, "Inventory", 20);
        title.setPosition({ position.x + 10, position.y - 30 });
        title.setFillColor(sf::Color::White);
        window.draw(title);
    }

	// Draw slots and items
    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < columns; ++col) {
            int slotIndex = row * columns + col;

            sf::RectangleShape slot(sf::Vector2f(slotSize, slotSize));
            slot.setPosition({ position.x + 10.f + col * slotSize, position.y + 10.f + row * slotSize });
            slot.setFillColor(sf::Color(80, 80, 80, 200));
            slot.setOutlineThickness(1.f);
            slot.setOutlineColor(sf::Color::White);

            window.draw(slot);

			// Draw item if exists
            InventoryItem* item = playerInventory.getItem(slotIndex);
            if (item && item->tileId != TILE_AIR && item->quantity > 0) {
                if (item->sprite) {
                    // Scale sprite to fit the panel slot, with small padding, and center it.
                    float padding = 6.f;
                    float avail = slotSize - 2.f * padding;
                    // guard against zero tileSize
                    float texW = (tileSize.x > 0) ? static_cast<float>(tileSize.x) : avail;
                    float texH = (tileSize.y > 0) ? static_cast<float>(tileSize.y) : avail;
                    float s = std::min(avail / texW, avail / texH);
                    item->sprite->setScale({ s, s });
                    // center inside slot
                    sf::Vector2f spriteSize(texW * s, texH * s);
                    sf::Vector2f spritePos = slot.getPosition() + sf::Vector2f((slotSize - spriteSize.x) / 2.f, (slotSize - spriteSize.y) / 2.f);
                    item->sprite->setPosition(spritePos);
                     window.draw(*item->sprite);
                }

				// Draw quantity if more than 1
                if (item->quantity > 1 && fontLoaded) {
                    sf::Text quantityText(font, std::to_string(item->quantity), 14);
                    quantityText.setFillColor(sf::Color::White);
                    quantityText.setPosition(
                        { slot.getPosition().x + slotSize - 20,
                        slot.getPosition().y + slotSize - 20 }
                    );
                    window.draw(quantityText);
                }
            }
        }
    }

    if (dragging && dragSprite) {
        window.draw(*dragSprite);
    }
}

void InventoryPanel::handleClick(const sf::Vector2f& mousePos) {
    if (!isVisible) return;

    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < columns; ++col) {
            sf::FloatRect slotRect(
                { position.x + 10.f + col * slotSize, position.y + 10.f + row * slotSize },
                { slotSize, slotSize }
            );

            if (slotRect.contains(mousePos)) {
                int slotIndex = row * columns + col;
                InventoryItem* src = playerInventory.getItem(slotIndex);
                if (!src || src->tileId == TILE_AIR)
					return;
                   
                

				// Search for free hotbar slot (0-8)
                int freeHotbar = -1;
                for (int i = 0; i < 9; ++i) {
                    InventoryItem* h = playerInventory.getItem(i);
                    if (h && (h->tileId == TILE_AIR || h->quantity == 0)) {
                        freeHotbar = i;
                        break;
                    }
                }

                if (freeHotbar != -1) {
					//Move to free hotbar slot
                    InventoryItem* dst = playerInventory.getItem(freeHotbar);
                    if (dst) {
                        dst->tileId = src->tileId;
                        dst->quantity = src->quantity;
                        dst->name = src->name;
                        dst->sprite = std::move(src->sprite);
						// Clear source slot
                        *src = InventoryItem();
						// Select the moved item in hotbar
                        playerInventory.setSelectedSlot(freeHotbar);
                    }
                }
                else {
                    int sel = playerInventory.getSelectedSlot();
                    InventoryItem* selSlotItem = playerInventory.getItem(sel);
                    if (selSlotItem) {
                        int tempId = selSlotItem->tileId;
                        int tempQty = selSlotItem->quantity;
                        std::string tempName = selSlotItem->name;
                        auto tempSprite = std::move(selSlotItem->sprite);

                        selSlotItem->tileId = src->tileId;
                        selSlotItem->quantity = src->quantity;
                        selSlotItem->name = src->name;
                        selSlotItem->sprite = std::move(src->sprite);

                        src->tileId = tempId;
                        src->quantity = tempQty;
                        src->name = tempName;
                        src->sprite = std::move(tempSprite);
                    }
                }

				// Update sprites after changes
                if (texture.getSize().x > 0 && tileSize.x > 0) {
                    playerInventory.updateSprites(texture, tileSize);
                }

                return;
            }
        }
    }
}

void InventoryPanel::handleMousePressed(const sf::Vector2f& mousePos, sf::Mouse::Button button) {
    if (!isVisible || button != sf::Mouse::Button::Left) return;

    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < columns; ++col) {
            sf::FloatRect slotRect(
                { position.x + 10.f + col * slotSize, position.y + 10.f + row * slotSize },
                { slotSize, slotSize }
            );
            if (slotRect.contains(mousePos)) {
                int slotIndex = row * columns + col;
                InventoryItem* src = playerInventory.getItem(slotIndex);
                if (!src || src->tileId == TILE_AIR) return;

                // start dragging
                dragging = true;
                dragSrcIndex = slotIndex;
                dragTileId = src->tileId;
                dragQuantity = src->quantity;

                // create drag sprite 
                if (texture.getSize().x > 0) {
                    dragSprite = std::make_unique<sf::Sprite>(texture);
                    if (src->sprite) {
                        dragSprite->setTextureRect(src->sprite->getTextureRect());
                    } else {
                        int texX = (dragTileId % 16) * tileSize.x;
                        int texY = (dragTileId / 16) * tileSize.y;
                        dragSprite->setTextureRect({ {texX, texY} , {static_cast<int>(tileSize.x), static_cast<int>(tileSize.y)} });
                    }
                    // scale to panel slot (same as draw)
                    float padding = 6.f;
                    float avail = slotSize - 2.f * padding;
                    float texW = (tileSize.x > 0) ? static_cast<float>(tileSize.x) : avail;
                    float texH = (tileSize.y > 0) ? static_cast<float>(tileSize.y) : avail;
                    float s = std::min(avail / texW, avail / texH);
                    dragSprite->setScale({ s, s });
                    // initial position
                    dragSprite->setPosition(mousePos - sf::Vector2f((texW*s)/2.f, (texH*s)/2.f));
                }
                return;
            }
        }
    }
}

void InventoryPanel::handleMouseMoved(const sf::Vector2f& mousePos) {
    if (!dragging || !dragSprite) return;
    // center cursor
    sf::FloatRect tr = dragSprite->getGlobalBounds();
    dragSprite->setPosition(mousePos - sf::Vector2f(tr.size.x / 2.f, tr.size.y / 2.f));
}

void InventoryPanel::handleMouseReleased(const sf::Vector2f& mousePos, sf::Mouse::Button button) {
    if (!dragging || button != sf::Mouse::Button::Left) {
        dragging = false;
        dragSprite.reset();
        dragSrcIndex = -1;
        dragTileId = TILE_AIR;
        return;
    }

    // Hotbar region (match Inventory::draw constants)
    const float hotbarSlot = 40.f;
    float startX = static_cast<float>(guiWindowSize.x) / 2.f - (9 * hotbarSlot) / 2.f;
    float y = static_cast<float>(guiWindowSize.y) - 50.f;
    sf::FloatRect hotbarRect({ startX, y }, { 9 * hotbarSlot, hotbarSlot });

    if (hotbarRect.contains(mousePos)) {
        int hbIndex = static_cast<int>((mousePos.x - startX) / hotbarSlot);
        if (hbIndex < 0) hbIndex = 0;
        if (hbIndex > 8) hbIndex = 8;

        InventoryItem* src = playerInventory.getItem(dragSrcIndex);
        if (src && src->tileId != TILE_AIR) {
            // find free hotbar slot
            int freeHotbar = -1;
            for (int i = 0; i < 9; ++i) {
                InventoryItem* h = playerInventory.getItem(i);
                if (h && (h->tileId == TILE_AIR || h->quantity == 0)) {
                    freeHotbar = i;
                    break;
                }
            }
            if (freeHotbar != -1) {
                InventoryItem* dst = playerInventory.getItem(freeHotbar);
                if (dst) {
                    dst->tileId = src->tileId;
                    dst->quantity = src->quantity;
                    dst->name = src->name;
                    dst->sprite = std::move(src->sprite);
                    *src = InventoryItem();
                    playerInventory.setSelectedSlot(freeHotbar);
                }
            } else {
                // swap with selected slot if no free
                int sel = playerInventory.getSelectedSlot();
                InventoryItem* selItem = playerInventory.getItem(sel);
                if (selItem) {
                    int tempId = selItem->tileId;
                    int tempQty = selItem->quantity;
                    std::string tempName = selItem->name;
                    auto tempSprite = std::move(selItem->sprite);

                    selItem->tileId = src->tileId;
                    selItem->quantity = src->quantity;
                    selItem->name = src->name;
                    selItem->sprite = std::move(src->sprite);

                    src->tileId = tempId;
                    src->quantity = tempQty;
                    src->name = tempName;
                    src->sprite = std::move(tempSprite);
                }
            }

            // update sprites
            if (texture.getSize().x > 0 && tileSize.x > 0) {
                playerInventory.updateSprites(texture, tileSize);
            }
        }
    }

    // end dragging
    dragging = false;
    dragSprite.reset();
    dragSrcIndex = -1;
    dragTileId = TILE_AIR;
    dragQuantity = 0;
}