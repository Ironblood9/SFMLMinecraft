#include <SFML/Graphics.hpp>
#include "TileMap.h"
#include <vector>
#include <cstdlib>

int main()
{
    sf::RenderWindow window(sf::VideoMode({ 800u, 600u }), "Mini Minecraft 2D Platformer - SFML3");
    window.setFramerateLimit(60);

    // 100x100 tile map
    const unsigned int width = 100u;
    const unsigned int height = 100u;
    const sf::Vector2u tileSize(32u, 32u);

    std::vector<int> tiles(width * height);

    // Rastgele harita (%70 boþ/çimen 0, %30 blok 1)
    for (unsigned int y = 0; y < height; ++y)
    {
        for (unsigned int x = 0; x < width; ++x)
        {
            tiles[x + y * width] = (rand() % 100 < 70) ? 0 : 1;
        }
    }

    TileMap map;
    if (!map.load("tileset.png", tileSize, tiles, width, height))
        return -1;

    // Kamera
    sf::View view(window.getDefaultView());

    // Oyuncu rectangle
    sf::RectangleShape player(sf::Vector2f(static_cast<float>(tileSize.x),
        static_cast<float>(tileSize.y)));
    player.setFillColor(sf::Color::Red);

    sf::Vector2f playerPos(100.f, 100.f); // baþlangýç pozisyonu
    sf::Vector2f playerVelocity(0.f, 0.f);

    const float moveSpeed = 4.f;
    const float jumpSpeed = -12.f;
    const float gravity = 0.5f;

    while (window.isOpen())
    {
        while (const std::optional<sf::Event> ev = window.pollEvent()) {
            if (ev->is<sf::Event::Closed>()) {
                window.close();
            }
        }

        // --- Input: sol/sað + jump ---
        playerVelocity.x = 0.f;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))
            playerVelocity.x = -moveSpeed;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
            playerVelocity.x = moveSpeed;

        // Jump sadece yerdeyse
        bool onGround = false;
        int tileX = static_cast<int>((playerPos.x + tileSize.x / 2) / tileSize.x);
        int tileY = static_cast<int>((playerPos.y + tileSize.y) / tileSize.y);

        if (tileY >= 0 && tileY < static_cast<int>(height) &&
            tileX >= 0 && tileX < static_cast<int>(width))
        {
            if (map.getTile(tileX, tileY) == 1)
                onGround = true;
        }

        if (onGround && sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space))
            playerVelocity.y = jumpSpeed;

        // Gravity
        playerVelocity.y += gravity;

        // Yeni pozisyon
        sf::Vector2f newPos = playerPos + playerVelocity;

        // --- Collision check: sadece alt taraf ---
        tileX = static_cast<int>((newPos.x + tileSize.x / 2) / tileSize.x);
        tileY = static_cast<int>((newPos.y + tileSize.y) / tileSize.y);
        if (tileY >= 0 && tileY < static_cast<int>(height) &&
            tileX >= 0 && tileX < static_cast<int>(width))
        {
            if (map.getTile(tileX, tileY) == 1)
            {
                newPos.y = tileY * tileSize.y - tileSize.y; // block üstüne oturt
                playerVelocity.y = 0.f;
                onGround = true;
            }
        }

        playerPos = newPos;
        player.setPosition(playerPos);

        // Kamera takip
        view.setCenter(playerPos + sf::Vector2f(tileSize.x / 2.f, tileSize.y / 2.f));
        window.setView(view);

        // --- Fare ile blok kýrma/koyma ---
        sf::Vector2i mousePixelPos = sf::Mouse::getPosition(window);
        sf::Vector2f mouseWorldPos = window.mapPixelToCoords(mousePixelPos);

        int mX = static_cast<int>(mouseWorldPos.x) / static_cast<int>(tileSize.x);
        int mY = static_cast<int>(mouseWorldPos.y) / static_cast<int>(tileSize.y);

        if (mX >= 0 && mX < static_cast<int>(width) &&
            mY >= 0 && mY < static_cast<int>(height))
        {
            if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
                map.setTile(static_cast<unsigned int>(mX),
                    static_cast<unsigned int>(mY), 0);
            if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Right))
                map.setTile(static_cast<unsigned int>(mX),
                    static_cast<unsigned int>(mY), 1);
        }

        // --- Çizim ---
        window.clear(sf::Color::Black);
        window.draw(map);
        window.draw(player);
        window.display();
    }

    return 0;
}





