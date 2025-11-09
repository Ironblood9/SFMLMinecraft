#pragma once
#include <SFML/Graphics.hpp>
#include "Inventory.h"

class InventoryPanel {
private:
    sf::RectangleShape background;
    sf::Texture texture;
    sf::Vector2u tileSize;
    bool isVisible;
    Inventory& playerInventory;

    // Grid settings
    int columns;
    int rows;
    float slotSize;
    sf::Vector2f position;

public:
    InventoryPanel(Inventory& inventory, const sf::Vector2u& windowSize);

    bool loadTexture(const std::string& texturePath);
    void toggle();
    bool getVisible() const { return isVisible; }
    void draw(sf::RenderWindow& window);
    void handleClick(const sf::Vector2f& mousePos);
    void updateLayout(const sf::Vector2u& windowSize);
};

