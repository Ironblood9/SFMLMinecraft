#include "Character.h"
#include <SFML/Window/Keyboard.hpp>

Character::Character(sf::Texture& texture)
    : sprite(texture), facingRight(true),
    isOnGround(false), isJumping(false),
    gravity(1500.0f), jumpVelocity(-600.0f),
    maxFallSpeed(1000.0f) {

    animations["idle"] = Animation({ 64, 64 }, 1, 0.2f, 0);
    animations["walk"] = Animation({ 64, 64 }, 4, 0.1f, 1);
    animations["jump"] = Animation({ 64, 64 }, 1, 0.15f, 2);
    animations["pickaxe_idle"] = Animation({ 64, 64 }, 1, 0.1f, 3);
    animations["pickaxe"] = Animation({ 64, 67 }, 4, 0.1f, 4);
    animations["attack3"] = Animation({ 64, 64 }, 4, 0.08f, 5);
    animations["slash"] = Animation({ 64, 64 }, 3, 0.1f, 6);
    animations["slash2"] = Animation({ 64, 64 }, 4, 0.1f, 7);
    animations["shield"] = Animation({ 64, 64 }, 3, 0.12f, 8);
    animations["bow"] = Animation({ 64, 64 }, 3, 0.12f, 9);
    animations["fall"] = Animation({ 64, 64 }, 1, 0.15f, 1);

    currentAnimation = "idle";
    sprite.setTextureRect(animations[currentAnimation].getTextureRect());
    sprite.setScale({ 1.5f, 1.5f });
    updateOrigin();
    sf::FloatRect spriteBounds = sprite.getLocalBounds();
    hitbox = sf::FloatRect({ 0.f, 0.f }, { spriteBounds.size.x * 0.7f, spriteBounds.size.y * 0.9f });
    updateHitbox();
}

void Character::update(float deltaTime) {
    previousPosition = sprite.getPosition();

    // Pickaxe animation
    if (mining ) {
        pickaxeAnimationTimer += deltaTime;

        // Animasyon cycle
        if (pickaxeAnimationTimer >= PICKAXE_ANIMATION_DURATION) {
            if (mining) {
                pickaxeAnimationTimer = 0.0f;
                animations["pickaxe"].reset();
            }
            else {
                pickaxeAnimationTimer = 0.0f;
            }
        }
    }

    // Gravity
    if (!isOnGround) {
        applyGravity(deltaTime);
    }

    // Friction 
    if (isOnGround) {
        float currentFriction = (mining) ? friction * 0.8f : friction;
        velocity.x *= currentFriction;
        if (std::abs(velocity.x) < 10.f) velocity.x = 0.f;
    }

    // Move
    sprite.move(velocity * deltaTime);

    // Animation
    animations[currentAnimation].update(deltaTime);
    sprite.setTextureRect(animations[currentAnimation].getTextureRect());

    updateHitbox();
    updateAnimationState();
}

void Character::applyGravity(float deltaTime) {
    velocity.y += gravity * deltaTime;

    // Max fall speed
    if (velocity.y > maxFallSpeed) {
        velocity.y = maxFallSpeed;
    }
}

void Character::jump() {
    if (isOnGround && !mining) { 
        velocity.y = jumpVelocity;
        isOnGround = false;
        isJumping = true;
    }
}

void Character::updateAnimationState()
{
    if (mining) {
        setAnimation("pickaxe");
        return;
    }

    if (!isOnGround) {
        if (velocity.y < 0) {
            setAnimation("jump");
        }
        else {
            setAnimation("fall");
        }
    }
    else {
        if (std::abs(velocity.x) > 0.1f) {
            setAnimation("walk");
        }
        else {
            setAnimation("idle");
        }
    }
}

void Character::handleInput()
{
    float currentMoveSpeed = mining ? moveSpeed * 0.6f : moveSpeed;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) {
        velocity.x = -currentMoveSpeed;

        if (facingRight) {
            facingRight = false;
            sprite.setScale({ -1.5f, 1.5f });
            updateOrigin();
        }
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) {
        velocity.x = currentMoveSpeed;

        if (!facingRight) {
            facingRight = true;
            sprite.setScale({ 1.5f, 1.5f });
        }
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space) && isOnGround && !mining) {
        jump();
    }
}

void Character::startMining() {
    if (!mining) {
        mining = true;
        pickaxeAnimationTimer = 0.0f;
        setAnimation("pickaxe");
        animations["pickaxe"].reset();
    }
}

void Character::stopMining() {
    mining = false;
}

bool Character::isMining() const {
    return mining;
}

bool Character::isPickaxeAnimationComplete() const {
    return pickaxeAnimationTimer >= PICKAXE_ANIMATION_DURATION;
}

void Character::resetPickaxeAnimation() {
    mining = false;
    pickaxeAnimationTimer = 0.0f;
}



//Origin!!!
void Character::updateOrigin() {
    sf::FloatRect bounds = sprite.getLocalBounds();
    sprite.setOrigin({ bounds.size.x / 2, bounds.size.y / 2 });
}

void Character::updateHitbox() {
    sf::FloatRect spriteBounds = sprite.getGlobalBounds();

    float hitboxX = spriteBounds.position.x + (spriteBounds.size.x - hitbox.size.x) / 2;
    float hitboxY = spriteBounds.position.y + spriteBounds.size.y - hitbox.size.y;

    hitbox.position = sf::Vector2f(hitboxX, hitboxY);
}

void Character::revertPosition() {
    sprite.setPosition(previousPosition);
    updateHitbox();
}

void Character::setOnGround(bool onGround) {
    isOnGround = onGround;
    if (onGround) {
        isJumping = false;
        velocity.y = 0;
    }
}

bool Character::getIsOnGround() const {
    return isOnGround;
}

const sf::FloatRect& Character::getHitbox() const {
    return hitbox;
}

void Character::setAnimation(const std::string& animName) {
    if (animations.find(animName) != animations.end() && currentAnimation != animName) {
        currentAnimation = animName;
        animations[currentAnimation].reset();
    }
}

void Character::setPosition(float x, float y) {
    sprite.setPosition({ x, y });
    updateHitbox();
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

void Character::stopMovement() {
    velocity.x = 0; 
    velocity.y = 0; 
    isMoving = false; 
}

sf::FloatRect Character::getGlobalBounds() const {
    return sprite.getGlobalBounds();
}

void Character::setPosition(const sf::Vector2f& position) {
    sprite.setPosition(position);
    updateHitbox();
}