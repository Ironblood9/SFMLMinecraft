#include "Character.h"


Character::Character(sf::Texture& texture)
    : sprite(texture), facingRight(true) {

    animations["idle"] = Animation({ 50, 90 }, 1, 0.2f, 0);
    animations["walk"] = Animation({ 64, 64 }, 4, 0.1f, 1);
    animations["jump"] = Animation({ 64, 64 }, 1, 0.15f, 2);
    animations["attack1"] = Animation({ 64, 64 }, 3, 0.1f, 3);
    animations["attack2"] = Animation({ 64, 64 }, 4, 0.08f, 4);
    animations["attack3"] = Animation({ 64, 64 }, 4, 0.08f, 5);
    animations["slash"] = Animation({ 64, 64 }, 3, 0.1f, 6);
    animations["slash2"] = Animation({ 64, 64 }, 4, 0.1f, 7);
    animations["shield"] = Animation({ 64, 64 }, 3, 0.12f, 8);
    animations["bow"] = Animation({ 64, 64 }, 3, 0.12f, 9);
    animations["fall"] = Animation({ 64, 64 }, 2, 0.15f, 10);


    currentAnimation = "idle";
    sprite.setTextureRect(animations[currentAnimation].getTextureRect());
    sprite.setScale({ 1.5f, 1.5f });
}

void Character::update(float deltaTime) {
    animations[currentAnimation].update(deltaTime);
    sprite.setTextureRect(animations[currentAnimation].getTextureRect());
    sprite.move(velocity * deltaTime);
}

void Character::handleInput() {
    velocity.x = 0;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) {
        velocity.x = -100.f;
        setAnimation("walk");
        facingRight = false;
        sprite.setScale({- 1.5f, 1.5f }); 
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) {
        velocity.x = 100.f;
        setAnimation("walk");
        facingRight = true;
        sprite.setScale({ 1.5f, 1.5f });
    }
    else {
        setAnimation("idle");
    }
}

void Character::setAnimation(const std::string& animName) {
    if (animations.find(animName) != animations.end() && currentAnimation != animName) {
        currentAnimation = animName;
        animations[currentAnimation].reset();
    }
}

void Character::setPosition(float x, float y) {
    sprite.setPosition({ x, y });
}

void Character::draw(sf::RenderWindow& window) const {
    window.draw(sprite);
}

sf::Vector2f Character::getPosition() const {
    return sprite.getPosition();
}


sf::Vector2f Character::getVelocity() const {
    return velocity;
}

void Character::setVelocity(const sf::Vector2f& newVelocity) {
    velocity = newVelocity;
}

sf::FloatRect Character::getGlobalBounds() const {
    return sprite.getGlobalBounds();
}

void Character::setPosition(const sf::Vector2f& position) {
    sprite.setPosition(position);
}


