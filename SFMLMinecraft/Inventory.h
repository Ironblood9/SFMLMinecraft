#pragma once
#include <SFML/Graphics.hpp>

struct InventoryItem {
    int tileId;
    int quantity;
    std::string name;
    std::unique_ptr<sf::Sprite> sprite; // SFML 3 uyumlu

    InventoryItem(int id = -1, int qty = 0, const std::string& n = "");
};

class Inventory {
private:
    std::vector<InventoryItem> items;
    int selectedSlot;
    int capacity;

public:
    Inventory(int capacity = 36);

    bool addItem(int tileId, int quantity = 1);
    bool removeItem(int slotIndex, int quantity = 1);
    InventoryItem* getItem(int slotIndex);
    void setSelectedSlot(int slot);
    int getSelectedSlot() const { return selectedSlot; }
    InventoryItem* getSelectedItem();
    void draw(sf::RenderWindow& window, const sf::Texture& texture);
    void updateSprites(const sf::Texture& texture, const sf::Vector2u& tileSize);

    const std::vector<InventoryItem>& getItems() const { return items; }
};

