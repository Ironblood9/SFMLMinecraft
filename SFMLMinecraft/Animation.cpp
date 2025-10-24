#include "Animation.h"

// Default constructor
Animation::Animation()
    : frameSize(0, 0), currentFrame(0), totalFrames(0), animationSpeed(0.f), elapsedTime(0.f), row(0) {
}

// Asýl constructor
Animation::Animation(sf::Vector2i frameSize, int totalFrames, float speed, int row)
    : frameSize(frameSize), currentFrame(0), totalFrames(totalFrames), animationSpeed(speed), elapsedTime(0.f), row(row) {
}

// Animasyonu güncelle
void Animation::update(float deltaTime) {
    if (totalFrames <= 1) return;
    elapsedTime += deltaTime;
    if (elapsedTime >= animationSpeed) {
        elapsedTime = 0.f;
        currentFrame = (currentFrame + 1) % totalFrames;
    }
}

sf::IntRect Animation::getTextureRect() const {
    return sf::IntRect({ currentFrame * frameSize.x, row * frameSize.y }, { frameSize.x, frameSize.y });
}

void Animation::reset() {
    currentFrame = 0;
    elapsedTime = 0.f;
}

void Animation::setRow(int newRow) {
    if (newRow != row) {
        row = newRow;
        reset();
    }
}

int Animation::getRow() const { return row; }
sf::Vector2i Animation::getFrameSize() const { return frameSize; }
int Animation::getTotalFrames() const { return totalFrames; }
void Animation::setSpeed(float speed) { animationSpeed = speed; }
float Animation::getSpeed() const { return animationSpeed; }
