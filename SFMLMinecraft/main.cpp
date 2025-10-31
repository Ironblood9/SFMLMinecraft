#include "TileMap.h"
#include "TileID.h"
#include "World.h"
#include "Character.h"
#include <vector>
#include <cstdlib>
#include <iostream>
#include <cmath>
#include <algorithm>

bool checkHitboxCollision(const sf::FloatRect& hitbox, const TileMap& map, const std::vector<int>& solidTiles) {
    int leftTile = static_cast<int>(hitbox.position.x / 46.f);
    int rightTile = static_cast<int>((hitbox.position.x + hitbox.size.x) / 46.f);
    int topTile = static_cast<int>(hitbox.position.y / 46.f);
    int bottomTile = static_cast<int>((hitbox.position.y + hitbox.size.y) / 46.f);

    for (int y = topTile; y <= bottomTile; ++y) {
        for (int x = leftTile; x <= rightTile; ++x) {
            if (x >= 0 && x < static_cast<int>(map.getWidth()) &&
                y >= 0 && y < static_cast<int>(map.getHeight())) {

                int tileID = map.getTile(static_cast<unsigned int>(x), static_cast<unsigned int>(y));

                if (std::find(solidTiles.begin(), solidTiles.end(), tileID) != solidTiles.end()) {
                    sf::FloatRect tileRect(
                        sf::Vector2f(static_cast<float>(x * 46), static_cast<float>(y * 46)),
                        sf::Vector2f(46.f, 46.f)
                    );

                    if (hitbox.findIntersection(tileRect)) {
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

void resolveCharacterCollision(Character& character, const TileMap& map, const std::vector<int>& solidTiles) {
    sf::FloatRect hitbox = character.getHitbox();

    // vertical collision
    sf::Vector2f horizontalMove(character.getVelocity().x * 0.016f, 0.f);
    sf::FloatRect horizontalTest = hitbox;
    horizontalTest.position.x += horizontalMove.x;

    if (checkHitboxCollision(horizontalTest, map, solidTiles)) {
        character.setVelocity(sf::Vector2f(0.f, character.getVelocity().y));
    }

    // Horizontal collision
    sf::Vector2f verticalMove(0.f, character.getVelocity().y * 0.016f);
    sf::FloatRect verticalTest = hitbox;
    verticalTest.position.y += verticalMove.y;

    if (checkHitboxCollision(verticalTest, map, solidTiles)) {
        // Ground collision
        if (character.getVelocity().y > 0) {
            character.setOnGround(true);
        }
        character.setVelocity(sf::Vector2f(character.getVelocity().x, 0.f));
    }
}


bool checkGroundCollision(const Character& character, const TileMap& map, const std::vector<int>& solidTiles) {
    sf::FloatRect hitbox = character.getHitbox();
    sf::FloatRect groundCheck = hitbox; //check under the hitbox
    groundCheck.position.y += 2.f;

    return checkHitboxCollision(groundCheck, map, solidTiles);
}

void drawHitbox(sf::RenderWindow& window, const Character& character) {
    sf::RectangleShape hitboxVisual;
    hitboxVisual.setSize(character.getHitbox().size);
    hitboxVisual.setPosition(character.getHitbox().position);
    hitboxVisual.setFillColor(sf::Color::Transparent);
    hitboxVisual.setOutlineColor(sf::Color::Red);
    hitboxVisual.setOutlineThickness(1.f);
    window.draw(hitboxVisual);
}

struct PickaxeBreak {
    bool active = false;
    float timer = 0.0f;
    int tileX = -1;
    int tileY = -1;
};

// Güncellenmiş: Pickaxe ile blok kırma fonksiyonu - 1.5 saniye sonra kır
bool handlePickaxeBreaking(Character& character, TileMap& map, int tileX, int tileY,
    const std::vector<int>& breakableTiles, const sf::Vector2u& tileSize, PickaxeBreak& pickaxeBreak, float deltaTime) {

    // Sol tıklama ile pickaxe kullanımı başlat
    static bool wasMousePressed = false;
    bool isMousePressed = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);

    if (isMousePressed && !wasMousePressed && !pickaxeBreak.active && !character.isUsingPickaxe()) {
        character.usePickaxe();
        pickaxeBreak.active = true;
        pickaxeBreak.timer = 0.0f;
        pickaxeBreak.tileX = tileX;
        pickaxeBreak.tileY = tileY;
        std::cout << "Pickaxe animation started, breaking in 1.5 seconds..." << std::endl;
    }
    wasMousePressed = isMousePressed;

    // Zamanlayıcıyı güncelle
    if (pickaxeBreak.active) {
        pickaxeBreak.timer += deltaTime;

        // 1.5 saniye sonra kırma işlemini yap
        if (pickaxeBreak.timer >= 1.0f) {
            // Karakterin merkez noktası
            sf::Vector2f characterCenter = character.getPosition();
            characterCenter.x += character.getGlobalBounds().size.x / 2.f;
            characterCenter.y += character.getGlobalBounds().size.y / 2.f;

            // Seçili karenin merkez noktası
            sf::Vector2f tileCenter(
                pickaxeBreak.tileX * tileSize.x + tileSize.x / 2.f,
                pickaxeBreak.tileY * tileSize.y + tileSize.y / 2.f
            );

            // Mesafe hesapla
            float distance = std::sqrt(
                std::pow(characterCenter.x - tileCenter.x, 2) +
                std::pow(characterCenter.y - tileCenter.y, 2)
            );

            // Maksimum kırma mesafesi (3 kare uzaklık)
            float maxBreakDistance = tileSize.x * 3.0f;

            bool blockBroken = false;
            if (distance <= maxBreakDistance) {
                if (pickaxeBreak.tileX >= 0 && pickaxeBreak.tileX < static_cast<int>(map.getWidth()) &&
                    pickaxeBreak.tileY >= 0 && pickaxeBreak.tileY < static_cast<int>(map.getHeight())) {

                    int tileID = map.getTile(static_cast<unsigned int>(pickaxeBreak.tileX),
                        static_cast<unsigned int>(pickaxeBreak.tileY));

                    // Eğer blok kırılabilir ise
                    if (std::find(breakableTiles.begin(), breakableTiles.end(), tileID) != breakableTiles.end()) {
                        // Bloku kır
                        map.setTile(static_cast<unsigned int>(pickaxeBreak.tileX),
                            static_cast<unsigned int>(pickaxeBreak.tileY), TILE_AIR);
                        std::cout << "Block broken at (" << pickaxeBreak.tileX << ", " << pickaxeBreak.tileY << ")" << std::endl;
                        blockBroken = true;
                    }
                }
            }
            else {
                std::cout << "Too far to break! Distance: " << distance << std::endl;
            }

            // Kırma işlemi tamamlandı, zamanlayıcıyı sıfırla
            pickaxeBreak.active = false;
            pickaxeBreak.timer = 0.0f;

            return blockBroken;
        }
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

    std::cout << "Generating world with liquids..." << std::endl;
    generateCleanTerrainWithLiquids(tiles, width, height);
    generateNiceTrees(tiles, width, height);
    generateSimpleDetails(tiles, width, height);
    std::cout << "World generation complete!" << std::endl;

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
    // Pickaxe ile kırılabilen bloklar
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

    // Pickaxe kırma zamanlayıcısı
    PickaxeBreak pickaxeBreak;

    sf::Clock clock;

    while (window.isOpen()) {
        float deltaTime = clock.restart().asSeconds();

        while (const std::optional<sf::Event> ev = window.pollEvent()) {
            if (ev->is<sf::Event::Closed>()) {
                window.close();
            }
        }

        // Input handling 
        character.handleInput();
        sf::Vector2i mousePixelPos = sf::Mouse::getPosition(window);
        sf::Vector2f mouseWorldPos = window.mapPixelToCoords(mousePixelPos);

        int mX = static_cast<int>(mouseWorldPos.x) / static_cast<int>(tileSize.x);
        int mY = static_cast<int>(mouseWorldPos.y) / static_cast<int>(tileSize.y);

        // Pickaxe ile blok kırma - 1.5 saniye sonra kır
        handlePickaxeBreaking(character, map, mX, mY, breakableTiles, tileSize, pickaxeBreak, deltaTime);

        // Character update 
        character.update(deltaTime);

        // Collision resolve 
        resolveCharacterCollision(character, map, solidTiles);

        // Ground checking
        if (checkGroundCollision(character, map, solidTiles)) {
            character.setOnGround(true);
        }
        else {
            character.setOnGround(false);
        }

        // Collision checking
        if (checkHitboxCollision(character.getHitbox(), map, solidTiles)) {
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

        // Camera
        sf::Vector2f cameraTarget = character.getPosition() + sf::Vector2f(
            character.getGlobalBounds().size.x / 2.f,
            character.getGlobalBounds().size.y / 2.f
        );
        sf::Vector2f cameraPos = view.getCenter();
        sf::Vector2f cameraVelocity = (cameraTarget - cameraPos) * 6.f * deltaTime;
        view.setCenter(cameraPos + cameraVelocity);
        window.setView(view);

        selectionBox.setPosition(sf::Vector2f(static_cast<float>(mX * tileSize.x),
            static_cast<float>(mY * tileSize.y)));

        // Render
        window.clear(sf::Color(120, 180, 240));
        window.draw(map);
        character.draw(window);
        drawHitbox(window, character);
        window.draw(selectionBox);
        window.display();
    }

    return 0;
}