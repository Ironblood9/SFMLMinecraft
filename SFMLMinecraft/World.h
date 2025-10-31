#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include "TileMap.h"
#include "TileID.h"

bool rectIntersects(const sf::FloatRect& rect1, const sf::FloatRect& rect2);
int  randomInt(int min, int max);
void generateWaterPool(std::vector<int>& tiles, unsigned int width, unsigned int height, int centerX, int centerY, int size);
void generateLavaPool(std::vector<int>& tiles, unsigned int width, unsigned int height, int centerX, int centerY, int size);
void generateUndergroundCave(std::vector<int>& tiles, unsigned int width, unsigned int height, int centerX, int centerY, int size);
void generateCleanTerrainWithLiquids(std::vector<int>& tiles, unsigned int width, unsigned int height);
void generateNiceTrees(std::vector<int>& tiles, unsigned int width, unsigned int height);
void generateSimpleDetails(std::vector<int>& tiles, unsigned int width, unsigned int height);
