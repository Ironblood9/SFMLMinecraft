#include <SFML/Graphics.hpp>
#include "TileMap.h"
#include <vector>
#include <cstdlib>
#include <iostream>
#include <cmath>


enum TileID {
    TILE_AIR = 0,      // weather
    TILE_GRASS = 1,    // grass
    TILE_STONE = 2,    // stone
    TILE_DIRT = 3,     // dirt
    TILE_GRASS_DIRT = 4, // grass dirt
    TILE_WOOD = 5      // wood
};

bool checkCollision(const sf::FloatRect& rect, const TileMap& map, const std::vector<int>& solidTiles) {
    unsigned int tileWidth = map.getTileSize().x;
    unsigned int tileHeight = map.getTileSize().y;
    unsigned int map_width = map.getWidth();
    unsigned int map_height = map.getHeight();

    int leftTile = static_cast<int>(rect.position.x / static_cast<float>(tileWidth));
    int rightTile = static_cast<int>((rect.position.x + rect.size.x - 1.f) / static_cast<float>(tileWidth));
    int topTile = static_cast<int>(rect.position.y / static_cast<float>(tileHeight));
    int bottomTile = static_cast<int>((rect.position.y + rect.size.y - 1.f) / static_cast<float>(tileHeight));

    leftTile = std::max(0, leftTile);
    rightTile = std::min(static_cast<int>(map_width) - 1, rightTile);
    topTile = std::max(0, topTile);
    bottomTile = std::min(static_cast<int>(map_height) - 1, bottomTile);

    if (rightTile < 0 || leftTile > static_cast<int>(map_width) - 1 ||
        bottomTile < 0 || topTile > static_cast<int>(map_height) - 1) {
        return false;
    }

    for (int y = topTile; y <= bottomTile; ++y) {
        for (int x = leftTile; x <= rightTile; ++x) {
            int tile = map.getTile(static_cast<unsigned int>(x), static_cast<unsigned int>(y));
            for (int solidTile : solidTiles) {
                if (tile == solidTile) {
                    return true;
                }
            }
        }
    }
    return false;
}

bool rectIntersects(const sf::FloatRect& rect1, const sf::FloatRect& rect2) {
    return !(rect1.position.x > rect2.position.x + rect2.size.x ||
        rect1.position.x + rect1.size.x < rect2.position.x ||
        rect1.position.y > rect2.position.y + rect2.size.y ||
        rect1.position.y + rect1.size.y < rect2.position.y);
}

int main() {
    sf::RenderWindow window(sf::VideoMode({ 800u, 600u }), "Mini Minecraft 2D - Custom Tileset");
    window.setFramerateLimit(60);

    const unsigned int width = 100u;
    const unsigned int height = 100u;
    const sf::Vector2u tileSize(32u, 32u);

    std::vector<int> tiles(width * height, TILE_AIR);

    int baseHeight = height - 15; 

    for (unsigned int x = 0; x < width; ++x) {
        // surface
        tiles[x + baseHeight * width] = TILE_GRASS;

        // below the surface
        for (unsigned int y = baseHeight + 1; y < baseHeight + 3; ++y) {
            if (y < height) tiles[x + y * width] = TILE_GRASS_DIRT;
        }

        // dirt
        for (unsigned int y = baseHeight + 3; y < baseHeight + 8; ++y) {
            if (y < height) tiles[x + y * width] = TILE_DIRT;
        }

        // stone
        for (unsigned int y = baseHeight + 8; y < height; ++y) {
            if (y < height) tiles[x + y * width] = TILE_STONE;
        }

        // Random
        if (x % 4 == 0 && x > 2 && x < width - 2) {
            int variation = (std::rand() % 5) - 2; // -2, -1, 0, 1, 2
            int newSurface = baseHeight + variation;

            if (newSurface >= 5 && newSurface < static_cast<int>(height) - 5) {
                
                for (unsigned int y = 0; y < height; ++y) {
                    unsigned int idx = x + y * width;
                    if (y >= static_cast<unsigned int>(newSurface - 2) && y <= static_cast<unsigned int>(newSurface + 8)) {
                        if (y == static_cast<unsigned int>(newSurface)) {
                            tiles[idx] = TILE_GRASS;
                        }
                        else if (y == static_cast<unsigned int>(newSurface + 1) || y == static_cast<unsigned int>(newSurface + 2)) {
                            tiles[idx] = TILE_GRASS_DIRT;
                        }
                        else if (y > static_cast<unsigned int>(newSurface + 2) && y <= static_cast<unsigned int>(newSurface + 7)) {
                            tiles[idx] = TILE_DIRT;
                        }
                        else if (y > static_cast<unsigned int>(newSurface + 7)) {
                            tiles[idx] = TILE_STONE;
                        }
                    }
                }
            }
        }
    }

    // Tree
    for (unsigned int i = 0; i < 15; ++i) {
        unsigned int treeX = std::rand() % (width - 4) + 2;
        int surfaceY = -1;

        for (unsigned int y = 0; y < height; ++y) {
            if (tiles[treeX + y * width] == TILE_GRASS) {
                surfaceY = static_cast<int>(y);
                break;
            }
        }

        if (surfaceY > 3) {
            for (int dy = 1; dy <= 4 + (std::rand() % 2); ++dy) {
                int trunkY = surfaceY - dy;
                if (trunkY >= 0) {
                    tiles[treeX + trunkY * width] = TILE_WOOD;
                }
            }

            int leafStartY = surfaceY - 5;
            for (int ly = leafStartY; ly >= leafStartY - 2; --ly) {
                for (int lx = treeX - 2; lx <= treeX + 2; ++lx) {
                    if (lx >= 0 && lx < static_cast<int>(width) && ly >= 0) {
                        if (std::abs(static_cast<float>(lx - static_cast<int>(treeX))) + std::abs(static_cast<float>(ly - (leafStartY - 1))) <= 3) {
                            if (tiles[lx + ly * width] == TILE_AIR) {
                                tiles[lx + ly * width] = TILE_GRASS; 
                            }
                        }
                    }
                }
            }
        }
    }

    TileMap map;
    if (!map.load("tileset.jpg", tileSize, tiles, width, height)) {
        std::cout << "Tileset failed to load! Check tileset.jpg file." << std::endl;
        return -1;
    }

    // Camera
    sf::View view(window.getDefaultView());

    // Player
    sf::RectangleShape player(sf::Vector2f(static_cast<float>(tileSize.x) - 4.f,
        static_cast<float>(tileSize.y) - 4.f));
    player.setFillColor(sf::Color::Blue);
    player.setOutlineColor(sf::Color::White);
    player.setOutlineThickness(1.f);

    sf::Vector2f playerPos(100.f, 50.f);
    sf::Vector2f playerVelocity(0.f, 0.f);

    const float moveSpeed = 0.3f;
    const float maxSpeed = 5.f;
    const float jumpSpeed = -10.f;
    const float gravity = 0.4f;
    const float friction = 0.85f;

    bool onGround = false;
    float jumpCooldown = 0.f;

    std::vector<int> solidTiles = {
        TILE_GRASS,
        TILE_STONE,
        TILE_DIRT,
        TILE_GRASS_DIRT,
        TILE_WOOD
    };

    sf::RectangleShape selectionBox(sf::Vector2f(static_cast<float>(tileSize.x),
        static_cast<float>(tileSize.y)));
    selectionBox.setFillColor(sf::Color::Transparent);
    selectionBox.setOutlineColor(sf::Color::Yellow);
    selectionBox.setOutlineThickness(2.f);

    int selectedBlock = TILE_GRASS;

    sf::Clock clock;

    while (window.isOpen()) {
        float deltaTime = clock.restart().asSeconds();

        while (const std::optional<sf::Event> ev = window.pollEvent()) {
            if (ev->is<sf::Event::Closed>()) {
                window.close();
            }

            if (auto* keyEvent = ev->getIf<sf::Event::KeyPressed>()) {
                if (keyEvent->code >= sf::Keyboard::Key::Num1 && keyEvent->code <= sf::Keyboard::Key::Num5) {
                    selectedBlock = static_cast<int>(keyEvent->code) - static_cast<int>(sf::Keyboard::Key::Num1) + 1;
                    std::cout << "Secilen blok: " << selectedBlock << std::endl;
                }

                if (keyEvent->code == sf::Keyboard::Key::Escape) {
                    window.close();
                }
            }
        }

        // INPUT HANDLING
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

        //Gravity
        if (!onGround) {
            playerVelocity.y += gravity;
        }

        sf::Vector2f newPos = playerPos;

        newPos.x += playerVelocity.x;
        sf::FloatRect horizontalBounds(newPos, player.getSize());
        if (checkCollision(horizontalBounds, map, solidTiles)) {
            newPos.x = playerPos.x;
            playerVelocity.x = 0.f;
        }

        newPos.y += playerVelocity.y;
        sf::FloatRect verticalBounds(newPos, player.getSize());
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
        player.setPosition(playerPos);

        // Window border
        unsigned int map_width = map.getWidth();
        if (playerPos.x < 0.f) playerPos.x = 0.f;
        if (playerPos.x > (map_width * tileSize.x) - player.getSize().x)
            playerPos.x = (map_width * tileSize.x) - player.getSize().x;
        if (playerPos.y < 0.f) playerPos.y = 0.f;

        sf::Vector2f cameraTarget = playerPos + player.getSize() / 2.f;
        sf::Vector2f cameraPos = view.getCenter();
        sf::Vector2f cameraVelocity = (cameraTarget - cameraPos) * 5.f * deltaTime;
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

            if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
                float distX = std::abs((mX * tileSize.x + tileSize.x / 2.f) - (playerPos.x + player.getSize().x / 2.f));
                float distY = std::abs((mY * tileSize.y + tileSize.y / 2.f) - (playerPos.y + player.getSize().y / 2.f));

                if (distX > tileSize.x * 1.5f || distY > tileSize.y * 1.5f) {
                    map.setTile(static_cast<unsigned int>(mX),
                        static_cast<unsigned int>(mY), TILE_AIR);
                }
            }
            if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Right)) {
                sf::FloatRect blockRect(
                    sf::Vector2f(static_cast<float>(mX * tileSize.x),
                        static_cast<float>(mY * tileSize.y)),
                    sf::Vector2f(static_cast<float>(tileSize.x),
                        static_cast<float>(tileSize.y))
                );

                if (!rectIntersects(blockRect, sf::FloatRect(player.getPosition(), player.getSize()))) {
                    map.setTile(static_cast<unsigned int>(mX),
                        static_cast<unsigned int>(mY), selectedBlock);
                }
            }
        }

        window.clear(sf::Color(135, 206, 235)); // Blue sky

        window.draw(map);
        window.draw(player);
        window.draw(selectionBox);

        window.display();
    }

    return 0;
}

