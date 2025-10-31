#include "CollisionManager.h"


bool CollisionManager::checkCollision(const sf::FloatRect& rect, const TileMap& map, const std::vector<int>& solidTiles) {
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

bool CollisionManager::checkHitboxCollision(const sf::FloatRect& hitbox, const TileMap& map, const std::vector<int>& solidTiles) {
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

void CollisionManager::resolveCharacterCollision(Character& character, const TileMap& map, const std::vector<int>& solidTiles) {
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


bool CollisionManager::checkGroundCollision(const Character& character, const TileMap& map, const std::vector<int>& solidTiles) {
    sf::FloatRect hitbox = character.getHitbox();
    sf::FloatRect groundCheck = hitbox; //check under the hitbox
    groundCheck.position.y += 2.f;

    return checkHitboxCollision(groundCheck, map, solidTiles);
}

void CollisionManager::drawHitbox(sf::RenderWindow& window, const Character& character) {
    sf::RectangleShape hitboxVisual;
    hitboxVisual.setSize(character.getHitbox().size);
    hitboxVisual.setPosition(character.getHitbox().position);
    hitboxVisual.setFillColor(sf::Color::Transparent);
    hitboxVisual.setOutlineColor(sf::Color::Red);
    hitboxVisual.setOutlineThickness(1.f);
    window.draw(hitboxVisual);
}