#include "TileMap.h"
#include "TileID.h"
#include "World.h"
#include "Character.h"
#include "CollisionManager.h"
#include <vector>
#include <cstdlib>
#include <iostream>
#include <cmath>
#include <algorithm>


struct MiningProgress {
    int tileX = -1;
    int tileY = -1;
    float progress = 0.0f;
    float requiredTime = 1.0f; 
    bool active = false;
};

bool handleMining(Character& character, TileMap& map, int tileX, int tileY,
    const std::vector<int>& breakableTiles, const sf::Vector2u& tileSize,
    MiningProgress& miningProgress, float deltaTime, bool isMousePressed) {

    //Distance check
    sf::Vector2f characterCenter = character.getPosition();
    characterCenter.x += character.getGlobalBounds().size.x / 2.f;
    characterCenter.y += character.getGlobalBounds().size.y / 2.f;

    sf::Vector2f tileCenter(
        tileX * tileSize.x + tileSize.x / 2.f,
        tileY * tileSize.y + tileSize.y / 2.f
    );

    float distance = std::sqrt(
        std::pow(characterCenter.x - tileCenter.x, 2) +
        std::pow(characterCenter.y - tileCenter.y, 2)
    );

    float maxMiningDistance = tileSize.x * 3.0f;

    if (distance > maxMiningDistance ||
        tileX < 0 || tileX >= static_cast<int>(map.getWidth()) ||
        tileY < 0 || tileY >= static_cast<int>(map.getHeight())) {

        character.stopMining();
        miningProgress.active = false;
        miningProgress.progress = 0.0f;
        return false;
    }

    int tileID = map.getTile(static_cast<unsigned int>(tileX), static_cast<unsigned int>(tileY));
    if (std::find(breakableTiles.begin(), breakableTiles.end(), tileID) == breakableTiles.end()) {
        character.stopMining();
        miningProgress.active = false;
        miningProgress.progress = 0.0f;
        return false;
    }

    if (!isMousePressed) {
        character.stopMining();
        miningProgress.active = false;
        miningProgress.progress = 0.0f;
        return false;
    }

    if (!miningProgress.active || miningProgress.tileX != tileX || miningProgress.tileY != tileY) {
        miningProgress.active = true;
        miningProgress.tileX = tileX;
        miningProgress.tileY = tileY;
        miningProgress.progress = 0.0f;
        character.startMining();
    }
    
    miningProgress.progress += deltaTime;

    if (miningProgress.progress >= miningProgress.requiredTime) {
        map.setTile(static_cast<unsigned int>(tileX), static_cast<unsigned int>(tileY), TILE_AIR);
        miningProgress.active = false;
        miningProgress.progress = 0.0f;
        character.stopMining();
        return true;
    }

    return false;
}

int main() {
    sf::RenderWindow window(sf::VideoMode({ 800u, 600u }), "2D Minecraft");
    window.setFramerateLimit(60);

    const unsigned int width = 100u;
    const unsigned int height = 60u;
    const sf::Vector2u tileSize(46u, 46u);

    std::vector<int> tiles(width * height, TILE_AIR);

    // Create World
    generateCleanTerrainWithLiquids(tiles, width, height);
    generateNiceTrees(tiles, width, height);
    generateSimpleDetails(tiles, width, height);

    sf::Texture characterTexture;
    if (!characterTexture.loadFromFile("assets/character_sheet.png")) {
        std::cerr << "Failed to load character_sheet.png!" << std::endl;
        return -1;
    }

    Character character(characterTexture);
    character.setPosition(150.f, 100.f);

    TileMap map;
    if (!map.load("assets/tileset.png", tileSize, tiles, width, height)) {
        std::cout << "Tileset failed to load! Check tileset file." << std::endl;
        return -1;
    }

    sf::View view(window.getDefaultView());

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

    sf::RectangleShape selectionBox(sf::Vector2f(static_cast<float>(tileSize.x),
        static_cast<float>(tileSize.y)));
    selectionBox.setFillColor(sf::Color::Transparent);
    selectionBox.setOutlineColor(sf::Color::Yellow);
    selectionBox.setOutlineThickness(2.f);

    // Mining progress
    MiningProgress miningProgress;

    sf::Clock clock;
    bool wasMousePressed = false;

    while (window.isOpen()) {
        float deltaTime = clock.restart().asSeconds();

        // Event handling
        while (const std::optional<sf::Event> ev = window.pollEvent()) {
            if (ev->is<sf::Event::Closed>()) {
                window.close();
            }
        }

        // Input handling
        character.handleInput();

        // Mouse position
        sf::Vector2i mousePixelPos = sf::Mouse::getPosition(window);
        sf::Vector2f mouseWorldPos = window.mapPixelToCoords(mousePixelPos);
        int mX = static_cast<int>(mouseWorldPos.x) / static_cast<int>(tileSize.x);
        int mY = static_cast<int>(mouseWorldPos.y) / static_cast<int>(tileSize.y);

        // Mining handling
        bool isMousePressed = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);
        handleMining(character, map, mX, mY, breakableTiles, tileSize, miningProgress, deltaTime, isMousePressed);

        // Character update
        character.update(deltaTime);

        // Collision resolution
        CollisionManager::resolveCharacterCollision(character, map, solidTiles);

        // Ground checking
        character.setOnGround(CollisionManager::checkGroundCollision(character, map, solidTiles));

        // Collision checking
        if (CollisionManager::checkHitboxCollision(character.getHitbox(), map, solidTiles)) {
            character.revertPosition();
        }

        // World border
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

        // Smooth camera
        sf::Vector2f cameraTarget = character.getPosition();
        sf::Vector2f cameraPos = view.getCenter();
        sf::Vector2f cameraMove = (cameraTarget - cameraPos) * 5.0f * deltaTime;
        view.setCenter(cameraPos + cameraMove);
        window.setView(view);

        // Selection box
        selectionBox.setPosition({ static_cast<float>(mX * tileSize.x), static_cast<float>(mY * tileSize.y) });

        // Render
        window.clear(sf::Color(120, 180, 240));
        window.draw(map);
        character.draw(window);
        window.draw(selectionBox);
        window.display();
    }

    return 0;
}
