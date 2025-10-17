#include "World.h"
#include "TileMap.h"
#include "TileID.h"

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
        for (int y = centerY; y <= centerY + size; ++y) {
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