#include "TileMap.h"
#include "TileID.h"
#include "World.h"
#include "Character.h"
#include "CollisionManager.h"
#include "ActionManager.h"
#include "Inventory.h"
#include "InventroyPanel.h"


int main() {
    sf::RenderWindow window(sf::VideoMode({ 800u, 600u }), "2D Minecraft");
    window.setFramerateLimit(60);

    const unsigned int width = 100u;
    const unsigned int height = 60u;
    const sf::Vector2u tileSize(46u, 46u);

    std::vector<int> tiles(width * height, TILE_AIR);

    // --- World generation ---
    generateCleanTerrainWithLiquids(tiles, width, height);
    generateNiceTrees(tiles, width, height);
    generateSimpleDetails(tiles, width, height);

    // --- Character ---
    sf::Texture characterTexture;
    if (!characterTexture.loadFromFile("assets/character_sheet.png")) {
        std::cerr << "Failed to load character_sheet.png!" << std::endl;
        return -1;
    }

    Character character(characterTexture);
    character.setPosition(150.f, 100.f);

    // --- TileMap ---
    TileMap map;
    if (!map.load("assets/tileset.png", tileSize, tiles, width, height)) {
        std::cout << "Tileset failed to load! Check tileset file." << std::endl;
        return -1;
    }

    // --- Inventory System ---
    Inventory playerInventory;
    playerInventory.updateSprites(map.getTileSet(), tileSize);

    InventoryPanel inventoryPanel(playerInventory, window.getSize());
    inventoryPanel.loadTexture("assets/tileset.png", tileSize);

    // optionally populate the inventory with tile IDs:
    inventoryPanel.populateWithTiles(TILE_CATEGORY_TERRAIN);
    inventoryPanel.populateWithTiles(TILE_CATEGORY_WOOD);
    inventoryPanel.populateWithTiles(TILE_CATEGORY_ORES);
    inventoryPanel.populateWithTiles(TILE_CATEGORY_DECORATIVE);

    // track previous selected slot so we can stop actions when selection changes
    int prevSelectedSlot = playerInventory.getSelectedSlot();


    sf::Font font;
    if (!font.openFromFile("assets/font.ttf")) {
        std::cout << "Font yüklenemedi, miktar yazıları olmadan devam ediliyor." << std::endl;
    }

    // --- View (Camera) ---
    sf::View view(window.getDefaultView());

    // --- Solid & Breakable tiles ---
    std::vector<int> solidTiles = {
        TILE_STONE, TILE_DIRT, TILE_GRASS, TILE_PLANKS, TILE_BRICKS,
        TILE_TNT, TILE_COBBLESTONE, TILE_BEDROCK, TILE_SAND, TILE_LOG,
        TILE_IRON_BLOCK, TILE_GOLD_BLOCK, TILE_DIAMOND_BLOCK, TILE_CHEST,
        TILE_COAL_ORE, TILE_IRON_ORE, TILE_BOOKSHELF, TILE_MOSSY_COBBLESTONE,
        TILE_OBSIDIAN, TILE_FURNACE, TILE_DIAMOND_ORE, TILE_RUBY_ORE,
        TILE_CRAFTING_TABLE, TILE_PUMPKIN, TILE_DARK_LOG, TILE_WHITE_LOG,
        TILE_MELON, TILE_CAKE, TILE_LAPIS_BLOCK, TILE_LAPIS_ORE,
        TILE_ENCHANTING_TABLE, TILE_LEAVES
    };

    std::vector<int> breakableTiles = {
        TILE_STONE, TILE_DIRT, TILE_GRASS, TILE_COBBLESTONE, TILE_COAL_ORE,
        TILE_IRON_ORE, TILE_DIAMOND_ORE, TILE_RUBY_ORE, TILE_LAPIS_ORE,
        TILE_LOG, TILE_DARK_LOG, TILE_WHITE_LOG, TILE_LEAVES, TILE_SAND,
        TILE_BOOKSHELF, TILE_PLANKS, TILE_TNT, TILE_MOSSY_COBBLESTONE,
        TILE_OBSIDIAN, TILE_FURNACE, TILE_CRAFTING_TABLE, TILE_PUMPKIN,
        TILE_MELON, TILE_CAKE, TILE_ENCHANTING_TABLE
    };

    // --- Selection Box ---
    sf::RectangleShape selectionBox(sf::Vector2f(static_cast<float>(tileSize.x),
        static_cast<float>(tileSize.y)));
    selectionBox.setFillColor(sf::Color::Transparent);
    selectionBox.setOutlineColor(sf::Color::Yellow);
    selectionBox.setOutlineThickness(2.f);

    // --- Action Manager ---
    ActionManager actionManager(3.0f);
    ActionManager::Progress miningProgress;

    sf::Clock clock;
    bool wasMousePressed = false;

    // --- Game Loop ---
    // --- Game Loop ---
    while (window.isOpen()) {
        float deltaTime = clock.restart().asSeconds();

        // --- Event Handling ---
        while (const std::optional<sf::Event> ev = window.pollEvent()) {
            if (ev->is<sf::Event::Closed>()) {
                window.close();
            }

            // E tuşu: envanteri aç/kapat
            if (ev->is<sf::Event::KeyPressed>()) {
                if (ev->getIf<sf::Event::KeyPressed>()->code == sf::Keyboard::Key::E) {
                    inventoryPanel.toggle();
                }

                // Between 1–9  hotbar slots selection
                auto key = ev->getIf<sf::Event::KeyPressed>()->code;
                if (key >= sf::Keyboard::Key::Num1 && key <= sf::Keyboard::Key::Num9) {
                    int slot = static_cast<int>(key) - static_cast<int>(sf::Keyboard::Key::Num1);
                    playerInventory.setSelectedSlot(slot);
                }

            }

            // Mouse click inventory interaction
            if (ev->is<sf::Event::MouseButtonPressed>()) {
                sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
                inventoryPanel.handleClick(mousePos);
            }
        }

        // --- Input ---
        character.handleInput();

        // detect selected slot change and stop current action if selection changed
        int currentSelectedSlot = playerInventory.getSelectedSlot();
        if (currentSelectedSlot != prevSelectedSlot) {
            actionManager.stopAction(character);
            prevSelectedSlot = currentSelectedSlot;
        }

        // update character held item from inventory selection each frame
        InventoryItem* selItem = playerInventory.getSelectedItem();
        int heldId = (selItem ? selItem->tileId : TILE_AIR);
        character.setHeldItem(heldId);

        // --- Mouse Position ---
        sf::Vector2i mousePixelPos = sf::Mouse::getPosition(window);
        sf::Vector2f mouseWorldPos = window.mapPixelToCoords(mousePixelPos);
        int mX = static_cast<int>(mouseWorldPos.x) / static_cast<int>(tileSize.x);
        int mY = static_cast<int>(mouseWorldPos.y) / static_cast<int>(tileSize.y);

        // --- Actions (left-click behaviour depends on selected tool)
        bool isLeftMousePressed = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);
        bool isRightMousePressed = sf::Mouse::isButtonPressed(sf::Mouse::Button::Right);

        if (!inventoryPanel.getVisible()) {
            character.handleInput();

            if (heldId == TOOL_SWORD) {
                // left mouse -> sword attack
                actionManager.handleSwordAttack(character, mX, mY, tileSize, deltaTime, isLeftMousePressed);
            }
            else if (heldId == TOOL_PICKAXE) {
                // left mouse -> mining
                actionManager.handleMining(character, map, mX, mY, breakableTiles, tileSize, deltaTime, isLeftMousePressed);
            }
            else {
                // default behavior: left mining, right sword
                actionManager.handleMining(character, map, mX, mY, breakableTiles, tileSize, deltaTime, isLeftMousePressed);
                actionManager.handleSwordAttack(character, mX, mY, tileSize, deltaTime, isRightMousePressed);
            }
        }
        else {
            character.stopMovement();
        }

        // --- Character Physics ---
        character.update(deltaTime);

        CollisionManager::resolveCharacterCollision(character, map, solidTiles);
        character.setOnGround(CollisionManager::checkGroundCollision(character, map, solidTiles));

        if (CollisionManager::checkHitboxCollision(character.getHitbox(), map, solidTiles)) {
            character.revertPosition();
        }

        // --- World Borders ---
        unsigned int map_width = map.getWidth();
        unsigned int map_height = map.getHeight();
        sf::Vector2f playerPos = character.getPosition();
        sf::FloatRect playerBounds = character.getGlobalBounds();

        if (playerPos.x < 0.f) character.setPosition(0.f, playerPos.y);
        if (playerPos.x > (map_width * tileSize.x) - playerBounds.size.x)
            character.setPosition((map_width * tileSize.x) - playerBounds.size.x, playerPos.y);
        if (playerPos.y < 0.f) character.setPosition(playerPos.x, 0.f);
        if (playerPos.y > (map_height * tileSize.y) - playerBounds.size.y) {
            character.setPosition(playerPos.x, (map_height * tileSize.y) - playerBounds.size.y);
            character.setOnGround(true);
        }

        // --- Camera ---
        sf::Vector2f cameraTarget = character.getPosition();
        sf::Vector2f cameraPos = view.getCenter();
        sf::Vector2f cameraMove = (cameraTarget - cameraPos) * 5.0f * deltaTime;
        view.setCenter(cameraPos + cameraMove);
        window.setView(view);

        // --- Selection Box ---
        selectionBox.setPosition({ static_cast<float>(mX * tileSize.x), static_cast<float>(mY * tileSize.y) });

        // --- Rendering ---
        window.clear(sf::Color(120, 180, 240));

		//World Draw 
        window.setView(view);
        window.draw(map);
        character.draw(window);
        window.draw(selectionBox);

        // Hitbar draw
        window.setView(window.getDefaultView());
        playerInventory.draw(window, map.getTileSet());
        //Inventrory Panel draw
        inventoryPanel.draw(window);
        window.setView(view);
        window.display();
    }

    return 0;
}
