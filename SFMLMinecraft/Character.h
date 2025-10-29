
#include <SFML/Graphics.hpp>
#include <string>
#include <unordered_map>
#include "Animation.h"

class Character {
public:
    Character(sf::Texture& texture);
    void update(float deltaTime);
    void handleInput();
    void draw(sf::RenderWindow& window) const;
    void setPosition(float x, float y);
    void setPosition(const sf::Vector2f& position);
    sf::Vector2f getPosition() const;
    void setVelocity(const sf::Vector2f& newVelocity);
    sf::Vector2f getVelocity() const;
    sf::FloatRect getGlobalBounds() const;
    const sf::FloatRect& getHitbox() const;
    void revertPosition();
    void setOnGround(bool onGround);
    bool getIsOnGround() const;

private:
    void updateHitbox();
    void updateAnimationState();
    void applyGravity(float deltaTime);
    void jump();
    void setAnimation(const std::string& animName);
    void updateOrigin();

    sf::Sprite sprite;
    std::unordered_map<std::string, Animation> animations;
    std::string currentAnimation;
    bool facingRight;
    sf::Vector2f velocity;
    sf::Vector2f previousPosition;
    sf::FloatRect hitbox;

    bool isOnGround;
    bool isJumping;
    float gravity;
    float jumpVelocity;
    float maxFallSpeed;
    const float moveSpeed = 300.f;
    const float friction = 0.88f;
};

