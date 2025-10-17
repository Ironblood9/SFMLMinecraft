#include "TileMap.h"
#include "TileID.h"
#include "World.h"
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


    sf::Vector2f playerPos(150.f, 30.f);
    sf::Vector2f playerVelocity(0.f, 0.f);
    TileMap map;
    // Player 
    sf::Texture playerTexture;
    if (!playerTexture.loadFromFile("assets/player.png")) {
        std::cerr << "Failed to load player.png!" << std::endl;
        return -1;
    }

    sf::IntRect playerFrame({ 0, 0 }, { 16, 44 });

    sf::Sprite playerSprite(playerTexture);
    playerSprite.setTextureRect(playerFrame);
    playerSprite.setPosition(playerPos);
    playerSprite.setScale({ 1.5f, 1.5f });


    if (!map.load("tileset.jpeg", tileSize, tiles, width, height)) {
        std::cout << "Tileset failed to load! Check tileset file." << std::endl;
        return -1;
    }

    // Camera
    sf::View view(window.getDefaultView());

    const float moveSpeed = 0.35f;
    const float maxSpeed = 6.f;
    const float jumpSpeed = -11.f;
    const float gravity = 0.45f;
    const float friction = 0.88f;

    bool onGround = false;
    float jumpCooldown = 0.f;

    // Solid tiles
    std::vector<int> solidTiles = {
        TILE_GRASS, TILE_DIRT, TILE_STONE, TILE_BEDROCK,
        TILE_COAL_ORE, TILE_IRON_ORE, TILE_GOLD_ORE, TILE_DIAMOND_ORE,
        TILE_LOG, TILE_LEAVES
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

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) {
            playerVelocity.x -= moveSpeed;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) {
            playerVelocity.x += moveSpeed;
        }

        if (playerVelocity.x > maxSpeed) playerVelocity.x = maxSpeed;
        if (playerVelocity.x < -maxSpeed) playerVelocity.x = -maxSpeed;

        playerVelocity.x *= friction;
        if (std::abs(playerVelocity.x) < 0.1f) playerVelocity.x = 0.f;

        if (jumpCooldown > 0.f) {
            jumpCooldown -= deltaTime;
        }

        if (onGround && sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space) && jumpCooldown <= 0.f) {
            playerVelocity.y = jumpSpeed;
            onGround = false;
            jumpCooldown = 0.2f;
        }

        if (!onGround) {
            playerVelocity.y += gravity;
        }

        // collision control
        sf::Vector2f newPos = playerPos;

        newPos.x += playerVelocity.x;
        sf::FloatRect horizontalBounds(newPos, playerSprite.getGlobalBounds().size);
        if (checkCollision(horizontalBounds, map, solidTiles)) {
            newPos.x = playerPos.x;
            playerVelocity.x = 0.f;
        }

        newPos.y += playerVelocity.y;
        sf::FloatRect verticalBounds(newPos, playerSprite.getGlobalBounds().size);
        if (checkCollision(verticalBounds, map, solidTiles)) {
            if (playerVelocity.y > 0) {
                newPos.y = playerPos.y;
                onGround = true;
                playerVelocity.y = 0.f;
            }
            else {
                newPos.y = playerPos.y;
                playerVelocity.y = 0.f;
            }
        }
        else {
            onGround = false;
        }

        playerPos = newPos;
        playerSprite.setPosition(playerPos);


        unsigned int map_width = map.getWidth();
        if (playerPos.x < 0.f) playerPos.x = 0.f;
        if (playerPos.x > (map_width * tileSize.x) - playerSprite.getGlobalBounds().size.x)
            playerPos.x = (map_width * tileSize.x) - playerSprite.getGlobalBounds().size.x;
        if (playerPos.y < 0.f) playerPos.y = 0.f;

        // Camera chasing
        sf::Vector2f cameraTarget = playerPos + playerSprite.getGlobalBounds().size / 2.f;
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

            float distX = std::abs((mX * tileSize.x + tileSize.x / 2.f) - (playerPos.x + playerSprite.getGlobalBounds().size.x / 2.f));
            float distY = std::abs((mY * tileSize.y + tileSize.y / 2.f) - (playerPos.y + playerSprite.getGlobalBounds().size.y / 2.f));

            // Block breaking
            if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
                if (distX > tileSize.x * 1.5f || distY > tileSize.y * 1.5f) {
                    map.setTile(static_cast<unsigned int>(mX),
                        static_cast<unsigned int>(mY), TILE_AIR);
                }
            }

            // Block putting
            else if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Right)) {
                if (distX > tileSize.x * 1.5f || distY > tileSize.y * 1.5f) {
                    int currentTile = map.getTile(static_cast<unsigned int>(mX),
                        static_cast<unsigned int>(mY));

                    if (currentTile == TILE_AIR) {
                        map.setTile(static_cast<unsigned int>(mX),
                            static_cast<unsigned int>(mY), TILE_DIRT);
                    }
                }
            }
        }


        // Draw
        window.clear(sf::Color(120, 180, 240));

        window.draw(map);
        window.draw(playerSprite);
        window.draw(selectionBox);

        window.display();
    }

    return 0;
}