#include "TileMap.h"
#include "TileID.h"
#include <vector>
#include <cstdlib>
#include <iostream>
#include <cmath>

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

// Random number generator
int randomInt(int min, int max) {
    return min + std::rand() % (max - min + 1);
}

void generateWaterPool(std::vector<int>& tiles, unsigned int width, unsigned int height, int centerX, int centerY, int size) {
    for (int x = centerX - size; x <= centerX + size; ++x) {
        for (int y = centerY; y <= centerY + size; ++y) { // Aþaðýya doðru oluþtur
            if (x >= 0 && x < static_cast<int>(width) && y >= 0 && y < static_cast<int>(height)) {
                float distance = std::sqrt(std::pow(x - centerX, 2) + std::pow(y - centerY, 2));
                if (distance < size) {
                    tiles[x + y * width] = TILE_WATER;
                }
            }
        }
    }
}

void generateLavaPool(std::vector<int>& tiles, unsigned int width, unsigned int height, int centerX, int centerY, int size) {
    for (int x = centerX - size; x <= centerX + size; ++x) {
        for (int y = centerY; y <= centerY + size; ++y) { 
            if (x >= 0 && x < static_cast<int>(width) && y >= 0 && y < static_cast<int>(height)) {
                float distance = std::sqrt(std::pow(x - centerX, 2) + std::pow(y - centerY, 2));
                if (distance < size) {
                    tiles[x + y * width] = TILE_LAVA;
                }
            }
        }
    }
}

void generateUndergroundCave(std::vector<int>& tiles, unsigned int width, unsigned int height, int centerX, int centerY, int size) {
    for (int x = centerX - size; x <= centerX + size; ++x) {
        for (int y = centerY - size / 2; y <= centerY + size / 2; ++y) {
            if (x >= 0 && x < static_cast<int>(width) && y >= 0 && y < static_cast<int>(height)) {
                float distance = std::sqrt(std::pow(x - centerX, 2) + std::pow(y - centerY, 2) * 1.5f);
                if (distance < size) {
                    tiles[x + y * width] = TILE_AIR; 
                }
            }
        }
    }
}

void generateCleanTerrainWithLiquids(std::vector<int>& tiles, unsigned int width, unsigned int height) {
    int baseHeight = height - 25;

    for (unsigned int x = 0; x < width; ++x) {
        int surfaceY = baseHeight;

        if (x % 4 == 0) {
            surfaceY += randomInt(-1, 1);
        }

        tiles[x + surfaceY * width] = TILE_GRASS;

        int dirtDepth = 3 + randomInt(0, 1);
        for (int y = surfaceY + 1; y < surfaceY + dirtDepth; ++y) {
            if (y < static_cast<int>(height)) {
                tiles[x + y * width] = TILE_DIRT;
            }
        }

        for (int y = surfaceY + dirtDepth; y < static_cast<int>(height) - 3; ++y) {
            if (y < static_cast<int>(height)) {
                if (y > surfaceY + dirtDepth + 5) {
                    if (randomInt(0, 100) < 5) {
                        tiles[x + y * width] = TILE_COAL_ORE;
                    }
                    else if (randomInt(0, 100) < 3 && y > surfaceY + 10) {
                        tiles[x + y * width] = TILE_IRON_ORE;
                    }
                    else if (randomInt(0, 100) < 2 && y > surfaceY + 15) {
                        tiles[x + y * width] = TILE_GOLD_ORE;
                    }
                    else if (randomInt(0, 100) < 1 && y > surfaceY + 20) {
                        tiles[x + y * width] = TILE_DIAMOND_ORE;
                    }
                    else {
                        tiles[x + y * width] = TILE_STONE;
                    }
                }
                else {
                    tiles[x + y * width] = TILE_STONE;
                }
            }
        }

        for (int y = static_cast<int>(height) - 3; y < static_cast<int>(height); ++y) {
            if (y < static_cast<int>(height)) {
                tiles[x + y * width] = TILE_BEDROCK;
            }
        }
    }

    std::cout << "Generating surface water pools..." << std::endl;
    for (int i = 0; i < 4; ++i) {
        int poolX = randomInt(15, width - 16);
        int surfaceY = -1;
        //Find surface height
        for (unsigned int y = 0; y < height; ++y) {
            if (tiles[poolX + y * width] == TILE_GRASS) {
                surfaceY = static_cast<int>(y);
                break;
            }
        }

        if (surfaceY > 0 && surfaceY < static_cast<int>(height) - 5) {
            std::cout << "Water pool at: " << poolX << ", " << surfaceY + 1 << std::endl;
            generateWaterPool(tiles, width, height, poolX, surfaceY + 1, randomInt(2, 4));
        }
    }

    std::cout << "Generating underground lava pools..." << std::endl;
    for (int i = 0; i < 6; ++i) {
        int lavaX = randomInt(10, width - 11);
        int lavaY = randomInt(height / 2 + 10, height - 15);

        std::cout << "Lava pool at: " << lavaX << ", " << lavaY << std::endl;
        generateUndergroundCave(tiles, width, height, lavaX, lavaY, randomInt(3, 5));
        generateLavaPool(tiles, width, height, lavaX, lavaY, randomInt(2, 4));
    }

    std::cout << "Generating underground water pools..." << std::endl;
    for (int i = 0; i < 5; ++i) {
        int waterX = randomInt(10, width - 11);
        int waterY = randomInt(height / 2 + 5, height - 10);

        std::cout << "Water cave at: " << waterX << ", " << waterY << std::endl;
        generateUndergroundCave(tiles, width, height, waterX, waterY, randomInt(3, 6));
        generateWaterPool(tiles, width, height, waterX, waterY, randomInt(2, 4));
    }
}

void generateNiceTrees(std::vector<int>& tiles, unsigned int width, unsigned int height) {
    for (unsigned int i = 0; i < 20; ++i) {
        unsigned int treeX = randomInt(5, width - 6);
        int surfaceY = -1;

        for (unsigned int y = 0; y < height; ++y) {
            if (tiles[treeX + y * width] == TILE_GRASS) {
                surfaceY = static_cast<int>(y);
                break;
            }
        }

        if (surfaceY > 5) {
            int trunkHeight = randomInt(4, 6);
            for (int dy = 1; dy <= trunkHeight; ++dy) {
                int trunkY = surfaceY - dy;
                if (trunkY >= 0) {
                    tiles[treeX + trunkY * width] = TILE_LOG;
                }
            }

            int leafStartY = surfaceY - trunkHeight;
            int leafSize = 2;

            for (int ly = leafStartY; ly >= leafStartY - 2; --ly) {
                for (int lx = treeX - leafSize; lx <= treeX + leafSize; ++lx) {
                    if (lx >= 0 && lx < static_cast<int>(width) && ly >= 0) {
                        if (std::abs(lx - static_cast<int>(treeX)) <= leafSize &&
                            std::abs(ly - leafStartY) <= 2) {
                            if (tiles[lx + ly * width] == TILE_AIR) {
                                tiles[lx + ly * width] = TILE_LEAVES;
                            }
                        }
                    }
                }
            }
        }
    }
}

void generateSimpleDetails(std::vector<int>& tiles, unsigned int width, unsigned int height) {
    for (unsigned int i = 0; i < 30; ++i) {
        unsigned int plantX = randomInt(2, width - 3);
        int surfaceY = -1;

        for (unsigned int y = 0; y < height; ++y) {
            if (tiles[plantX + y * width] == TILE_GRASS) {
                surfaceY = static_cast<int>(y);
                break;
            }
        }

        if (surfaceY > 0) {
            int plantY = surfaceY - 1;
            if (plantY >= 0 && tiles[plantX + plantY * width] == TILE_AIR) {
                if (randomInt(0, 100) < 70) {
                    tiles[plantX + plantY * width] = TILE_GRASS;
                }
                else {
                    tiles[plantX + plantY * width] = (randomInt(0, 1) == 0 ? TILE_FLOWER_RED : TILE_FLOWER_YELLOW);
                }
            }
        }
    }
}

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

    TileMap map;
    if (!map.load("tileset.jpeg", tileSize, tiles, width, height)) {
        std::cout << "Tileset failed to load! Check tileset file." << std::endl;
        return -1;
    }

    // Camera
    sf::View view(window.getDefaultView());

    // Player
    sf::RectangleShape player(sf::Vector2f(static_cast<float>(tileSize.x) - 6.f,
        static_cast<float>(tileSize.y) - 6.f));
    player.setFillColor(sf::Color(255, 100, 100));
    player.setOutlineColor(sf::Color::White);
    player.setOutlineThickness(1.f);

    sf::Vector2f playerPos(150.f, 30.f);
    sf::Vector2f playerVelocity(0.f, 0.f);

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

        unsigned int map_width = map.getWidth();
        if (playerPos.x < 0.f) playerPos.x = 0.f;
        if (playerPos.x > (map_width * tileSize.x) - player.getSize().x)
            playerPos.x = (map_width * tileSize.x) - player.getSize().x;
        if (playerPos.y < 0.f) playerPos.y = 0.f;

        // Camera chasing
        sf::Vector2f cameraTarget = playerPos + player.getSize() / 2.f;
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

            float distX = std::abs((mX * tileSize.x + tileSize.x / 2.f) - (playerPos.x + player.getSize().x / 2.f));
            float distY = std::abs((mY * tileSize.y + tileSize.y / 2.f) - (playerPos.y + player.getSize().y / 2.f));

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
        window.draw(player);
        window.draw(selectionBox);

        window.display();
    }

    return 0;
}