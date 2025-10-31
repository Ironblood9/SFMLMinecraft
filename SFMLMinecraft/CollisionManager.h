#pragma once
#include <SFML/Graphics.hpp>
#include "Character.h"
#include "TileMap.h"

class CollisionManager {
public:
    static bool checkHitboxCollision(const sf::FloatRect& hitbox,
        const TileMap& map,
        const std::vector<int>& solidTiles);

    static void resolveCharacterCollision(Character& character,
        const TileMap& map,
        const std::vector<int>& solidTiles);

    static bool checkGroundCollision(const Character& character,
        const TileMap& map,
        const std::vector<int>& solidTiles);

    static void drawHitbox(sf::RenderWindow& window,
        const Character& character);

    static bool checkCollision(const sf::FloatRect& rect, 
        const TileMap& map, 
        const std::vector<int>& solidTiles);

};

