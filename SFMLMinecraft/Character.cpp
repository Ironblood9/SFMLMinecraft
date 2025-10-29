#include "Character.h"
#include <SFML/Window/Keyboard.hpp>
#include <cmath>

Character::Character(sf::Texture& texture)
    : sprite(texture), facingRight(true),
    isOnGround(false), isJumping(false),
    gravity(1500.0f), jumpVelocity(-600.0f),
    maxFallSpeed(1000.0f) {

    animations["idle"] = Animation({ 64, 64 }, 1, 0.2f, 0);
    animations["walk"] = Animation({ 64, 64 }, 4, 0.1f, 1);
    animations["jump"] = Animation({ 64, 64 }, 1, 0.15f, 1);
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
    updateOrigin();
    // Hitbox boyutunu sprite'a göre ayarla
    sf::FloatRect spriteBounds = sprite.getLocalBounds();
    hitbox = sf::FloatRect({ 0.f, 0.f }, { spriteBounds.size.x * 0.7f, spriteBounds.size.y * 0.9f });
    updateHitbox();
}

void Character::update(float deltaTime) {
    previousPosition = sprite.getPosition();

    // Yer çekimi uygula (eðer yerde deðilse)
    if (!isOnGround) {
        applyGravity(deltaTime);
    }

    // Sürtünme uygula (yatay eksende)
    if (isOnGround) {
        velocity.x *= friction;
        if (std::abs(velocity.x) < 10.f) velocity.x = 0.f;
    }

    // Hareketi uygula
    sprite.move(velocity * deltaTime);

    // Animasyonu güncelle
    animations[currentAnimation].update(deltaTime);
    sprite.setTextureRect(animations[currentAnimation].getTextureRect());

    updateHitbox();
    updateAnimationState();
}

void Character::applyGravity(float deltaTime) {
    velocity.y += gravity * deltaTime;

    // Maksimum düþüþ hýzýný sýnýrla
    if (velocity.y > maxFallSpeed) {
        velocity.y = maxFallSpeed;
    }
}

void Character::jump() {
    if (isOnGround) {
        velocity.y = jumpVelocity;
        isOnGround = false;
        isJumping = true;
    }
}

void Character::updateAnimationState() {
    if (!isOnGround) {
        if (velocity.y < 0) {
            setAnimation("jump"); // Yukarý zýplýyor
        }
        else {
            setAnimation("fall"); // Aþaðý düþüyor
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

void Character::handleInput() {
    // Yatay hareket
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) {
        velocity.x = -moveSpeed;

        if (facingRight) {
            facingRight = false;
            sprite.setScale({ -1.5f, 1.5f });
            updateOrigin();
        }
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) {
        velocity.x = moveSpeed;

        if (!facingRight) {
            facingRight = true;
            sprite.setScale({ 1.5f, 1.5f });
        }
    }

    // Zýplama
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space) && isOnGround) {
        jump();
    }
}

void Character::updateOrigin() {
    sf::FloatRect bounds = sprite.getLocalBounds();
    sprite.setOrigin({ bounds.size.x / 2, bounds.size.y / 2 }); // Merkeze origin
}

void Character::updateHitbox() {
    sf::FloatRect spriteBounds = sprite.getGlobalBounds();

    // Hitbox'ý sprite'ýn ALT kýsmýna hizala - BU KRÝTÝK DEÐÝÞÝKLÝK
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
        velocity.y = 0; // Yere deðdiðinde dikey hýzý sýfýrla
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

sf::FloatRect Character::getGlobalBounds() const {
    return sprite.getGlobalBounds();
}

void Character::setPosition(const sf::Vector2f& position) {
    sprite.setPosition(position);
    updateHitbox();
}