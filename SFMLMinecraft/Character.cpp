#include "Character.h"


Character::Character(sf::Texture& texture)
    : sprite(texture), facingRight(true) {

    animations["idle"] = Animation(sf::Vector2i(64, 64), 4, 0.2f, 0);
    animations["walk"] = Animation(sf::Vector2i(64, 64), 6, 0.1f, 1);
    animations["attack"] = Animation(sf::Vector2i(64, 64), 6, 0.08f, 2);
    animations["mine"] = Animation(sf::Vector2i(64, 64), 6, 0.1f, 3);

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


