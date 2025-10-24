#pragma once
#include <SFML/Graphics.hpp>
#include <map>
#include <string>
#include "Animation.h"

class Character {
private:
    sf::Sprite sprite;
    std::map<std::string, Animation> animations;
    std::string currentAnimation;
    sf::Vector2f velocity;
    bool facingRight;

public:
    Character(sf::Texture& texture);

    void update(float deltaTime);
    void handleInput();
    void setAnimation(const std::string& animName);
    void setPosition(float x, float y);
    void setPosition(const sf::Vector2f& position);
    void draw(sf::RenderWindow& window) const;
    void setVelocity(const sf::Vector2f& newVelocity);

    sf::Vector2f getPosition() const;
    sf::FloatRect getGlobalBounds() const;
    sf::Vector2f getVelocity() const;
};

