#include "TileMap.h"
#include "TileID.h"
#include "World.h"
#include "Character.h"
#include <vector>
#include <cstdlib>
#include <iostream>
#include <cmath>

int main() {
    sf::RenderWindow window(sf::VideoMode({ 800u, 600u }), "2D Minecraft");
    window.setFramerateLimit(60);

    const unsigned int width = 100u;
    const unsigned int height = 60u;
    const sf::Vector2u tileSize(46u, 46u);

    std::vector<int> tiles(width * height, TILE_AIR);

    // Create World
    std::cout << "Generating world with liquids..." << std::endl;
    generateCleanTerrainWithLiquids(tiles, width, height);
    generateNiceTrees(tiles, width, height);
    generateSimpleDetails(tiles, width, height);
    std::cout << "World generation complete!" << std::endl;
    std::cout << "Features: Diamond ore (rare), Water pools, Lava pools underground" << std::endl;

    sf::Texture characterTexture;
    if (!characterTexture.loadFromFile("assets/character_sheet.png")) { 
        std::cerr << "Failed to load character_sheet.png!" << std::endl;
        return -1;
    }

    // Create Character with sprite sheet
    Character character(characterTexture);
    character.setPosition(150.f, 30.f);

    TileMap map;
    if (!map.load("assets/tileset.png", tileSize, tiles, width, height)) {
        std::cout << "Tileset failed to load! Check tileset file." << std::endl;
        return -1;
    }

    // Camera
    sf::View view(window.getDefaultView());

    // Physics variables
    sf::Vector2f playerVelocity(0.f, 0.f);
    const float moveSpeed = 300.f;
    const float maxSpeed = 300.f;
    const float jumpSpeed = -600.f;
    const float gravity = 1500.f;
    const float friction = 0.88f;

    bool onGround = false;
    float jumpCooldown = 0.f;

    // Solid tiles
    std::vector<int> solidTiles = {
    TILE_STONE, TILE_DIRT, TILE_GRASS, TILE_PLANKS, TILE_BRICKS,
    TILE_TNT, TILE_COBBLESTONE, TILE_BEDROCK, TILE_SAND, TILE_LOG,
    TILE_IRON_BLOCK, TILE_GOLD_BLOCK, TILE_DIAMOND_BLOCK, TILE_CHEST,
    TILE_COAL_ORE, TILE_IRON_ORE, TILE_BOOKSHELF, TILE_MOSSY_COBBLESTONE,
    TILE_OBSIDIAN, TILE_FURNACE, TILE_DIAMOND_ORE, TILE_RUBY_ORE,
    TILE_CRAFTING_TABLE, TILE_PUMPKIN, TILE_DARK_LOG, TILE_WHITE_LOG,
    TILE_MELON, TILE_CAKE, TILE_LAPIS_BLOCK, TILE_LAPIS_ORE,
    TILE_ENCHANTING_TABLE,TILE_LEAVES
    };

    sf::RectangleShape selectionBox(sf::Vector2f(static_cast<float>(tileSize.x),
        static_cast<float>(tileSize.y)));
    selectionBox.setFillColor(sf::Color::Transparent);
    selectionBox.setOutlineColor(sf::Color::Yellow);
    selectionBox.setOutlineThickness(2.f);

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

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) {
            playerVelocity.x = -moveSpeed;
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) {
            playerVelocity.x = moveSpeed;
        }
        else {
            playerVelocity.x *= friction;
            if (std::abs(playerVelocity.x) < 10.f) playerVelocity.x = 0.f;
        }

        // Hız limit
        if (playerVelocity.x > maxSpeed) playerVelocity.x = maxSpeed;
        if (playerVelocity.x < -maxSpeed) playerVelocity.x = -maxSpeed;

        // Jump control
        if (jumpCooldown > 0.f) {
            jumpCooldown -= deltaTime;
        }

        if (onGround && sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space) && jumpCooldown <= 0.f) {
            playerVelocity.y = jumpSpeed;
            onGround = false;
            jumpCooldown = 0.2f;
        }

        // Gravity
        if (!onGround) {
            playerVelocity.y += gravity * deltaTime;
        }

        sf::Vector2f newPos = character.getPosition();

        newPos.x += playerVelocity.x * deltaTime;
        sf::FloatRect horizontalBounds(newPos, {
            character.getGlobalBounds().size.x,
            character.getGlobalBounds().size.y
            });

        if (checkCollision(horizontalBounds, map, solidTiles)) {
            newPos.x = character.getPosition().x;
            playerVelocity.x = 0.f;
        }

        newPos.y += playerVelocity.y * deltaTime;
        sf::FloatRect verticalBounds(newPos, {
            character.getGlobalBounds().size.x,
            character.getGlobalBounds().size.y
            });

        if (checkCollision(verticalBounds, map, solidTiles)) {
            if (playerVelocity.y > 0) {
                newPos.y = character.getPosition().y;
                onGround = true;
                playerVelocity.y = 0.f;
            }
            else {
                newPos.y = character.getPosition().y;
                playerVelocity.y = 0.f;
            }
        }
        else {
            onGround = false;
        }

        // Position update
        character.setPosition(newPos.x, newPos.y);

        // World border
        unsigned int map_width = map.getWidth();
        sf::Vector2f playerPos = character.getPosition();
        if (playerPos.x < 0.f) character.setPosition(0.f, playerPos.y);
        if (playerPos.x > (map_width * tileSize.x) - character.getGlobalBounds().size.x)
            character.setPosition((map_width * tileSize.x) - character.getGlobalBounds().size.x, playerPos.y);
        if (playerPos.y < 0.f) character.setPosition(playerPos.x, 0.f);

        // Character update
        character.update(deltaTime);

        // Camera
        sf::Vector2f cameraTarget = character.getPosition() + sf::Vector2f(
            character.getGlobalBounds().size.x / 2.f,
            character.getGlobalBounds().size.y / 2.f
        );
        sf::Vector2f cameraPos = view.getCenter();
        sf::Vector2f cameraVelocity = (cameraTarget - cameraPos) * 6.f * deltaTime;
        view.setCenter(cameraPos + cameraVelocity);
        window.setView(view);

        sf::Vector2i mousePixelPos = sf::Mouse::getPosition(window);
        sf::Vector2f mouseWorldPos = window.mapPixelToCoords(mousePixelPos);

        int mX = static_cast<int>(mouseWorldPos.x) / static_cast<int>(tileSize.x);
        int mY = static_cast<int>(mouseWorldPos.y) / static_cast<int>(tileSize.y);

        selectionBox.setPosition(sf::Vector2f(static_cast<float>(mX * tileSize.x),
            static_cast<float>(mY * tileSize.y)));

        if (mX >= 0 && mX < static_cast<int>(map_width) &&
            mY >= 0 && mY < static_cast<int>(map.getHeight())) {

            float distX = std::abs((mX * tileSize.x + tileSize.x / 2.f) -
                (character.getPosition().x + character.getGlobalBounds().size.x / 2.f));
            float distY = std::abs((mY * tileSize.y + tileSize.y / 2.f) -
                (character.getPosition().y + character.getGlobalBounds().size.y / 2.f));

            if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
                if (distX > tileSize.x * 2.0f || distY > tileSize.y * 2.0f) {
                    map.setTile(static_cast<unsigned int>(mX),
                        static_cast<unsigned int>(mY), TILE_AIR);
                }
            }

            else if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Right)) {
                if (distX > tileSize.x * 2.0f || distY > tileSize.y * 2.0f) {
                    int currentTile = map.getTile(static_cast<unsigned int>(mX),
                        static_cast<unsigned int>(mY));

                    if (currentTile == TILE_AIR) {
                        map.setTile(static_cast<unsigned int>(mX),
                            static_cast<unsigned int>(mY), TILE_DIRT);
                    }
                }
            }
        }

        window.clear(sf::Color(120, 180, 240));
        window.draw(map);
        character.draw(window);
        window.draw(selectionBox);
        window.display();
    }

    return 0;
}
