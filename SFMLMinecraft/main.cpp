#include <SFML/Graphics.hpp>
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

// Rastgele sayý üretici
int randomInt(int min, int max) {
    return min + std::rand() % (max - min + 1);
}

// Çevre oluþturma fonksiyonlarý
void generateBasicTerrain(std::vector<int>& tiles, unsigned int width, unsigned int height) {
    int baseHeight = height - 20;

    for (unsigned int x = 0; x < width; ++x) {
        // Yüzey: Çimen
        tiles[x + baseHeight * width] = TILE_GRASS;

        // Yüzey altý: Toprak
        for (unsigned int y = baseHeight + 1; y < baseHeight + 4; ++y) {
            if (y < height) tiles[x + y * width] = TILE_DIRT;
        }

        // Taþ katmaný
        for (unsigned int y = baseHeight + 4; y < height - 5; ++y) {
            if (y < height) {
                // Maden damarlarý
                if (randomInt(0, 100) < 3) tiles[x + y * width] = TILE_COAL_ORE;
                else if (randomInt(0, 100) < 2) tiles[x + y * width] = TILE_IRON_ORE;
                else if (randomInt(0, 100) < 1) tiles[x + y * width] = TILE_GOLD_ORE;
                else tiles[x + y * width] = TILE_STONE;
            }
        }

        // En alt: Bedrock
        for (unsigned int y = height - 5; y < height; ++y) {
            if (y < height) {
                if (y == height - 1) tiles[x + y * width] = TILE_BEDROCK;
                else if (randomInt(0, 100) < 80) tiles[x + y * width] = TILE_BEDROCK;
                else tiles[x + y * width] = TILE_STONE;
            }
        }

        // Yüzey varyasyonlarý
        if (x % 3 == 0) {
            int variation = randomInt(-3, 3);
            int newSurface = baseHeight + variation;

            if (newSurface >= 5 && newSurface < static_cast<int>(height) - 5) {
                for (unsigned int y = 0; y < height; ++y) {
                    unsigned int idx = x + y * width;
                    if (y == static_cast<unsigned int>(newSurface)) {
                        tiles[idx] = TILE_GRASS;
                    }
                    else if (y > static_cast<unsigned int>(newSurface) && y <= static_cast<unsigned int>(newSurface) + 3) {
                        tiles[idx] = TILE_DIRT;
                    }
                }
            }
        }
    }
}

void generateTrees(std::vector<int>& tiles, unsigned int width, unsigned int height) {
    for (unsigned int i = 0; i < 25; ++i) {
        unsigned int treeX = randomInt(5, width - 6);
        int surfaceY = -1;

        // Yüzey yüksekliðini bul
        for (unsigned int y = 0; y < height; ++y) {
            if (tiles[treeX + y * width] == TILE_GRASS) {
                surfaceY = static_cast<int>(y);
                break;
            }
        }

        if (surfaceY > 5) {
            // Aðaç gövdesi
            int trunkHeight = randomInt(4, 7);
            for (int dy = 1; dy <= trunkHeight; ++dy) {
                int trunkY = surfaceY - dy;
                if (trunkY >= 0) {
                    tiles[treeX + trunkY * width] = TILE_LOG;
                }
            }

            // Yapraklar
            int leafStartY = surfaceY - trunkHeight - 1;
            for (int ly = leafStartY; ly >= leafStartY - 3; --ly) {
                for (int lx = treeX - 2; lx <= treeX + 2; ++lx) {
                    if (lx >= 0 && lx < static_cast<int>(width) && ly >= 0) {
                        // Yaprak þekli (elmas)
                        if (std::abs(lx - static_cast<int>(treeX)) + std::abs(ly - (leafStartY - 1)) <= 3) {
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





void generateSurfaceDetails(std::vector<int>& tiles, unsigned int width, unsigned int height) {
    // Çiçekler ve bitkiler
    for (unsigned int i = 0; i < 50; ++i) {
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
            if (plantY >= 0) {
                if (randomInt(0, 100) < 60) {
                    tiles[plantX + plantY * width] = TILE_GRASS;
                }
                else if (randomInt(0, 100) < 30) {
                    tiles[plantX + plantY * width] = (randomInt(0, 1) == 0 ? TILE_FLOWER_RED : TILE_FLOWER_YELLOW);
                }
            }
        }
    }
}

int main() {
    sf::RenderWindow window(sf::VideoMode({ 800u, 600u }), "2D Minecraft");
    window.setFramerateLimit(60);

    const unsigned int width = 120u;
    const unsigned int height = 80u;
    const sf::Vector2u tileSize(46u, 46u);

    std::vector<int> tiles(width * height, TILE_AIR);

    // Dünya oluþturma
    std::cout << "Generating world..." << std::endl;
    generateBasicTerrain(tiles, width, height);
    generateTrees(tiles, width, height);
    generateSurfaceDetails(tiles, width, height);
    std::cout << "World generation complete!" << std::endl;

    TileMap map;
    if (!map.load("tileset.jpeg", tileSize, tiles, width, height)) {
        std::cout << "Tileset failed to load! Check tileset file." << std::endl;
        return -1;
    }

    // Kamera
    sf::View view(window.getDefaultView());

    // Oyuncu
    sf::RectangleShape player(sf::Vector2f(static_cast<float>(tileSize.x) - 6.f,
        static_cast<float>(tileSize.y) - 6.f));
    player.setFillColor(sf::Color(255, 100, 100)); // Kýrmýzýmsý karakter
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

    // Katý tile'lar
    std::vector<int> solidTiles = {
        TILE_GRASS, TILE_DIRT, TILE_STONE, TILE_COBBLESTONE, TILE_PLANKS,
        TILE_BEDROCK, TILE_SAND, TILE_GRAVEL, TILE_COAL_ORE,
        TILE_IRON_ORE, TILE_GOLD_ORE, TILE_DIAMOND_ORE, TILE_LAPIS_ORE, TILE_LOG, TILE_LEAVES, TILE_BRICKS,
       TILE_MOSSY_COBBLESTONE, TILE_OBSIDIAN, TILE_IRON_BLOCK,
       TILE_GOLD_BLOCK, TILE_DIAMOND_BLOCK, TILE_LAPIS_BLOCK, TILE_BOOKSHELF, TILE_PUMPKIN,
       TILE_MELON, TILE_CAKE, TILE_ENCHANTING_TABLE,
       TILE_CRAFTING_TABLE, TILE_FURNACE, TILE_CHEST
    };

    // Sývý tile'lar
    std::vector<int> liquidTiles = {
        TILE_WATER, TILE_LAVA
    };

    // Seçim kutusu
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

        // Giriþ iþleme
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) {
            playerVelocity.x -= moveSpeed;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) {
            playerVelocity.x += moveSpeed;
        }

        // Hýz sýnýrlama
        if (playerVelocity.x > maxSpeed) playerVelocity.x = maxSpeed;
        if (playerVelocity.x < -maxSpeed) playerVelocity.x = -maxSpeed;

        // Sürtünme
        playerVelocity.x *= friction;
        if (std::abs(playerVelocity.x) < 0.1f) playerVelocity.x = 0.f;

        // Zýplama
        if (jumpCooldown > 0.f) {
            jumpCooldown -= deltaTime;
        }

        if (onGround && sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space) && jumpCooldown <= 0.f) {
            playerVelocity.y = jumpSpeed;
            onGround = false;
            jumpCooldown = 0.2f;
        }

        // Yer çekimi
        if (!onGround) {
            playerVelocity.y += gravity;
        }

        // Çarpýþma kontrolü
        sf::Vector2f newPos = playerPos;

        // Yatay çarpýþma
        newPos.x += playerVelocity.x;
        sf::FloatRect horizontalBounds(newPos, player.getSize());
        if (checkCollision(horizontalBounds, map, solidTiles)) {
            newPos.x = playerPos.x;
            playerVelocity.x = 0.f;
        }

        // Dikey çarpýþma
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

        // Pencere sýnýrlarý
        unsigned int map_width = map.getWidth();
        if (playerPos.x < 0.f) playerPos.x = 0.f;
        if (playerPos.x > (map_width * tileSize.x) - player.getSize().x)
            playerPos.x = (map_width * tileSize.x) - player.getSize().x;
        if (playerPos.y < 0.f) playerPos.y = 0.f;

        // Kamera takibi
        sf::Vector2f cameraTarget = playerPos + player.getSize() / 2.f;
        sf::Vector2f cameraPos = view.getCenter();
        sf::Vector2f cameraVelocity = (cameraTarget - cameraPos) * 6.f * deltaTime;
        view.setCenter(cameraPos + cameraVelocity);
        window.setView(view);

        // Blok düzenleme
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
            }
        }

        // Çizim
        window.clear(sf::Color(120, 180, 240)); // Daha açýk mavi gökyüzü

        window.draw(map);
        window.draw(player);
        window.draw(selectionBox);

        window.display();
    }

    return 0;
}