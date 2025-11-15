#include "InventroyPanel.h"
#include "TileID.h"

InventoryPanel::InventoryPanel(Inventory& inventory, const sf::Vector2u& windowSize)
    : playerInventory(inventory), isVisible(false), columns(10), rows(5), slotSize(46.f) {
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
        sf::Text title(font, "Envanter", 20);
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
                    item->sprite->setPosition(slot.getPosition() + sf::Vector2f(5.f, 5.f));
                    item->sprite->setScale({ 0.8f, 0.8f });
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
}

void InventoryPanel::handleClick(const sf::Vector2f& mousePos) {
    if (!isVisible) return;

    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < columns; ++col) {
            sf::FloatRect slotRect(
                { position.x + 10.f + col * slotSize,position.y + 10.f + row * slotSize },
                { slotSize,slotSize }
            );

            if (slotRect.contains(mousePos)) {
                int slotIndex = row * columns + col;
				// eklenecek etkileþimler burada
                break;
            }
        }
    }
}