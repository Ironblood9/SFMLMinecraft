#include "Inventory.h"
#include "TileID.h"

Inventory::Inventory(int capacity)
    : capacity(capacity), selectedSlot(0)
{
    items.resize(capacity);

    // Default items - 1. slot swrod, 2. slot picaxe
    items[0] = InventoryItem(TOOL_SWORD, 1, "Sword");
    items[1] = InventoryItem(TOOL_PICKAXE, 1, "Pickaxe");
}

bool Inventory::addItem(int tileId, int quantity) {
	//If same item exists, stack it
    for (auto& item : items) {
        if (item.tileId == tileId && item.quantity < 64) {
            item.quantity += quantity;
            return true;
        }
    }
    // Fýnd empty slot
    for (auto& item : items) {
        if (item.tileId == TILE_AIR) {
            item.tileId = tileId;
            item.quantity = quantity;

            // Set name based on tileId
            switch (tileId) {
            case TILE_STONE: item.name = "Stone"; break;
            case TILE_DIRT:  item.name = "Dirt"; break;
            case TILE_GRASS: item.name = "Grass"; break;
            case TILE_LOG:   item.name = "Wood"; break;
            default:         item.name = "Unknown"; break;
            }
            return true;
        }
    }
	
    return false; 
}

bool Inventory::removeItem(int slotIndex, int quantity) {
    if (slotIndex < 0 || slotIndex >= capacity) return false;

    items[slotIndex].quantity -= quantity;
    if (items[slotIndex].quantity <= 0) {
        items[slotIndex] = InventoryItem(); // reset to air
    }
    return true;
}

InventoryItem* Inventory::getItem(int slotIndex) {
    if (slotIndex < 0 || slotIndex >= capacity) return nullptr;
    return &items[slotIndex];
}

void Inventory::setSelectedSlot(int slot) {
    if (slot >= 0 && slot < 9) { 
        selectedSlot = slot;
    }
}

InventoryItem* Inventory::getSelectedItem() {
    return getItem(selectedSlot);
}

void Inventory::draw(sf::RenderWindow& window, const sf::Texture& texture) {
    float slotSize = 50.f;
    float startX = 400.f - (9 * slotSize) / 2.f;
    float y = 550.f;

	// hotbar background
    sf::RectangleShape hotbarBg(sf::Vector2f(9 * slotSize + 10, slotSize + 10));
    hotbarBg.setPosition({ startX - 5, y - 5 });
    hotbarBg.setFillColor(sf::Color(50, 50, 50, 180));
    window.draw(hotbarBg);

    for (int i = 0; i < 9; ++i) {
		// Slot frame
        sf::RectangleShape slot(sf::Vector2f(slotSize, slotSize));
        slot.setPosition({ startX + i * slotSize, y });
        slot.setFillColor(sf::Color(70, 70, 70, 200));
        slot.setOutlineThickness(2.f);
        slot.setOutlineColor(i == selectedSlot ? sf::Color::Yellow : sf::Color::White);
        window.draw(slot);

        auto& item = items[i];
        if (item.tileId != TILE_AIR && item.quantity > 0) {
            if (item.sprite) {
                item.sprite->setPosition({ startX + i * slotSize + 2, y + 2 });
                item.sprite->setScale({ 0.8f, 0.8f });
                window.draw(*item.sprite);
            }

            if (item.quantity > 1) {
                static sf::Font font;
                static bool fontLoaded = false;
                if (!fontLoaded) {
                    font.openFromFile("assets/font.ttf");
                    fontLoaded = true;
                }

                sf::Text quantityText(font, std::to_string(static_cast<int>(item.quantity)), 12);
                quantityText.setFillColor(sf::Color::White);
                quantityText.setPosition({ startX + i * slotSize + slotSize - 15, y + slotSize - 15 });
                window.draw(quantityText);
            }

        }
    }
}

void Inventory::updateSprites(const sf::Texture& texture, const sf::Vector2u& tileSize) {
    for (auto& item : items) {
        if (item.tileId != TILE_AIR) {
            item.sprite = std::make_unique<sf::Sprite>(texture);
            int texX = static_cast<int>((item.tileId % 16) * tileSize.x);
            int texY = static_cast<int>((item.tileId / 16) * tileSize.y);
            item.sprite->setTextureRect(sf::IntRect(
                { static_cast<int>(texX),static_cast<int>(texY) },
                { static_cast<int>(tileSize.x),static_cast<int>(tileSize.y) }
            ));
        }
        else {
            item.sprite.reset(); 
        }
    }
}

// InventoryItem constructor
InventoryItem::InventoryItem(int id, int qty, const std::string& n)
    : tileId(id), quantity(qty), name(n) {
}
