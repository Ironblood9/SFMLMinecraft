#include "InventroyPanel.h"
#include "TileID.h"

InventoryPanel::InventoryPanel(Inventory& inventory, const sf::Vector2u& windowSize)
    : playerInventory(inventory), isVisible(false), columns(9), rows(4), slotSize(60.f) {
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

bool InventoryPanel::loadTexture(const std::string& texturePath) {
    return texture.loadFromFile(texturePath);
}

void InventoryPanel::toggle() {
    isVisible = !isVisible;
}

void InventoryPanel::draw(sf::RenderWindow& window) {
    if (!isVisible) return;

    window.draw(background);

    // Slots çiz
    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < columns; ++col) {
            int slotIndex = row * columns + col;

            sf::RectangleShape slot(sf::Vector2f(slotSize, slotSize));
            slot.setPosition({ position.x + 10.f + col * slotSize, position.y + 10.f + row * slotSize });
            slot.setFillColor(sf::Color(100, 100, 100, 200));
            slot.setOutlineThickness(1.f);
            slot.setOutlineColor(sf::Color::White);

            window.draw(slot);

            // Item çiz
            InventoryItem* item = playerInventory.getItem(slotIndex);
            if (item && item->tileId != TILE_AIR && item->quantity > 0 && item->sprite) {
                item->sprite->setPosition(slot.getPosition() + sf::Vector2f(5.f, 5.f));
                item->sprite->setScale({ 0.7f, 0.7f });
                window.draw(*item->sprite);
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
                // Týklanan slot ile ilgili iþlemler
                // Örneðin: item seçme, taþýma vs.
                break;
            }
        }
    }
}