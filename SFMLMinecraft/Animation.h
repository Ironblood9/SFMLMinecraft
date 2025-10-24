#pragma once
#include <SFML/Graphics.hpp>

class Animation {
private:
    sf::Vector2i frameSize;
    int currentFrame;
    int totalFrames;
    float animationSpeed;
    float elapsedTime;
    int row;

public:
    Animation(); 
    Animation(sf::Vector2i frameSize, int totalFrames, float speed, int row = 0);

    void update(float deltaTime);
    sf::IntRect getTextureRect() const;
    void reset();
    void setRow(int newRow);
    int getRow() const;
    sf::Vector2i getFrameSize() const;
    int getTotalFrames() const;
    void setSpeed(float speed);
    float getSpeed() const;
};


