#include "ActionManager.h"

ActionManager::ActionManager(float distanceMultiplier)
    : distanceMultiplier(distanceMultiplier) {
}

void ActionManager::stopAction(Character& character) {
    if (currentAction == ActionType::Mining) {
        character.stopMining();
    }
    progress.active = false;
    progress.progress = 0.0f;
    progress.tileX = -1;
    progress.tileY = -1;
    currentAction = ActionType::None;
}

bool ActionManager::handleMining(Character& character, TileMap& map, int tileX, int tileY,
    const std::vector<int>& breakableTiles, const sf::Vector2u& tileSize,
    float deltaTime, bool isMousePressed) {

    // If we were mining but mouse is no longer pressed, stop
    if (currentAction == ActionType::Mining && !isMousePressed) {
        stopAction(character);
        return false;
    }

    // If no mouse press and no active action, do nothing
    if (!isMousePressed && currentAction == ActionType::None) {
        return false;
    }

    sf::Vector2f characterCenter = character.getPosition();
    characterCenter.x += character.getGlobalBounds().size.x / 2.f;
    characterCenter.y += character.getGlobalBounds().size.y / 2.f;

    sf::Vector2f tileCenter(
        tileX * tileSize.x + tileSize.x / 2.f,
        tileY * tileSize.y + tileSize.y / 2.f
    );

    float distance = std::sqrt(
        std::pow(characterCenter.x - tileCenter.x, 2) +
        std::pow(characterCenter.y - tileCenter.y, 2)
    );

    float maxDistance = tileSize.x * distanceMultiplier;

    // Distance and map bounds check
    if (distance > maxDistance ||
        tileX < 0 || tileX >= static_cast<int>(map.getWidth()) ||
        tileY < 0 || tileY >= static_cast<int>(map.getHeight())) {

        // Only stop if we were actually mining this target
        if (progress.active && (progress.tileX != tileX || progress.tileY != tileY)) {
            stopAction(character);
        }
        return false;
    }

    // Check if breakable
    int tileID = map.getTile(static_cast<unsigned int>(tileX), static_cast<unsigned int>(tileY));
    if (std::find(breakableTiles.begin(), breakableTiles.end(), tileID) == breakableTiles.end()) {
        if (progress.active) {
            stopAction(character);
        }
        return false;
    }

    // If we have an active mining action on a different block, stop it
    if (progress.active && (progress.tileX != tileX || progress.tileY != tileY)) {
        stopAction(character);
    }

    // Start new mining action
    if (!progress.active && isMousePressed) {
        progress.active = true;
        progress.tileX = tileX;
        progress.tileY = tileY;
        progress.progress = 0.0f;
        progress.requiredTime = 1.0f; // 1 second to break
        currentAction = ActionType::Mining;
        character.startMining();
    }

    // Update progress if active
    if (progress.active && isMousePressed) {
        progress.progress += deltaTime;

        // Check if completed
        if (progress.progress >= progress.requiredTime) {
            map.setTile(static_cast<unsigned int>(tileX), static_cast<unsigned int>(tileY), TILE_AIR);
            stopAction(character);
            return true;
        }
    }

    return false;
}