#pragma once
#include <SFML/Graphics.hpp>
#include "Animation.h"
#include "TileID.h"

class Character {
public:
    Character(sf::Texture& texture);

    void update(float deltaTime);
    void handleInput();
    void draw(sf::RenderWindow& window) const;

    void setPosition(float x, float y);
    void setPosition(const sf::Vector2f& position);
    sf::Vector2f getPosition() const;
    sf::Vector2f getVelocity() const;
    void setVelocity(const sf::Vector2f& newVelocity);
    void stopMovement();
    sf::FloatRect getGlobalBounds() const;
    const sf::FloatRect& getHitbox() const;

    void setOnGround(bool onGround);
    bool getIsOnGround() const;
    void revertPosition();
    // Pickaxe animation methods
    void startMining();
    void stopMining();
    bool isMining() const;
    bool isPickaxeAnimationComplete() const;
    void resetPickaxeAnimation();

    // Sword animation methods
    void startSwingingSword();
    void stopSwingingSword();
    bool isSwingingSword() const;
    bool isSwordAnimationComplete() const;
    void resetSwordAnimation();
    float getSwordAnimationDuration() const { return SWORD_ANIMATION_DURATION; }

    // affects idle & input behavior
    void setHeldItem(int tileOrToolId);
    int getHeldItem() const { return heldItemId; }

private:
    void applyGravity(float deltaTime);
    void jump();
    void updateAnimationState();
    void setAnimation(const std::string& animName);
    void updateOrigin();
    void updateHitbox();

    sf::Sprite sprite;
    std::unordered_map<std::string, Animation> animations;
    std::string currentAnimation;

    sf::Vector2f velocity{ 0.f, 0.f };
    sf::Vector2f previousPosition{ 0.f, 0.f };
    sf::FloatRect hitbox;

    bool facingRight;
    bool isOnGround;
    bool isJumping;
    bool isMoving;

    // Physics
    float gravity;
    float jumpVelocity;
    float maxFallSpeed;

    // Animations
    bool mining = false; // for pickaxe
    float pickaxeAnimationTimer = 0.0f;

    bool swingingSword = false; // for sword
    float swordAnimationTimer = 0.0f;

    // Held item id from inventory
    int heldItemId = TILE_AIR;

    // Constants
    static constexpr float moveSpeed = 400.0f;
    static constexpr float friction = 0.85f;
    static constexpr float PICKAXE_ANIMATION_DURATION = 1.0f;
    static constexpr float SWORD_ANIMATION_DURATION = 0.4f;
};