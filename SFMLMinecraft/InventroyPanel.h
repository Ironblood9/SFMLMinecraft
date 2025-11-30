#pragma once
#include <SFML/Graphics.hpp>
#include "Inventory.h"
#include "TileID.h"

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

    sf::Vector2u guiWindowSize;
    bool dragging = false;
    int dragSrcIndex = -1;
    std::unique_ptr<sf::Sprite> dragSprite;
    int dragTileId = TILE_AIR;
    int dragQuantity = 0;

public:
    InventoryPanel(Inventory& inventory, const sf::Vector2u& windowSize);

    bool loadTexture(const std::string& texturePath, const sf::Vector2u& tileSize);
    void populateWithTiles(const std::vector<int>& tileIds);

    void toggle();
    bool getVisible() const { return isVisible; }
    void draw(sf::RenderWindow& window);
    void handleClick(const sf::Vector2f& mousePos);
    void updateLayout(const sf::Vector2u& windowSize);

    void handleMousePressed(const sf::Vector2f& mousePos, sf::Mouse::Button button);
    void handleMouseReleased(const sf::Vector2f& mousePos, sf::Mouse::Button button);
    void handleMouseMoved(const sf::Vector2f& mousePos);
};

