#pragma once
#include "Character.h"
#include "TileMap.h"
#include "TileID.h"


class ActionManager {
public:
    enum class ActionType {
        None,
        Mining,
        SwordSwing,
        Building
    };

    struct Progress {
        int tileX = -1;
        int tileY = -1;
        float progress = 0.0f;
        float requiredTime = 1.0f;
        bool active = false;
    };

    explicit ActionManager(float distanceMultiplier = 3.0f);

    bool handleMining(Character& character, TileMap& map, int tileX, int tileY,
        const std::vector<int>& breakableTiles, const sf::Vector2u& tileSize,
        float deltaTime, bool isMousePressed);

    void stopAction(Character& character);

    ActionType getCurrentAction() const { return currentAction; }
    const Progress& getProgress() const { return progress; }

private:
    Progress progress;
    ActionType currentAction = ActionType::None;
    float distanceMultiplier;
};

//bool handleSwordSwing(Character& character, float deltaTime);
//bool handleBuilding(Character& character, TileMap& map, ...);

