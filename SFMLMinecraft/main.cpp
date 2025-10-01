
#include <SFML/Graphics.hpp>
#include <optional>

int main() {
    sf::RenderWindow window(sf::VideoMode({ 800,600 }), "Minecraft");
    window.setFramerateLimit(60);

    sf::RectangleShape block({ 32.f, 32.f });
    block.setFillColor(sf::Color::Magenta);
    block.setPosition({ 200.f, 200.f });

    sf::Clock clock;
    float speed = 200.f;

    while (window.isOpen()) {
        while (const std::optional<sf::Event> ev = window.pollEvent()) {
            if (ev->is<sf::Event::Closed>()) {
                window.close();
            }
        }

        float dt = clock.restart().asSeconds();

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) block.move({ -speed * dt, 0.f });
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) block.move({ speed * dt, 0.f });
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) block.move({ 0.f, -speed * dt });
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) block.move({ 0.f,  speed * dt });


        window.clear(sf::Color::Transparent);
        window.draw(block);
        window.display();
    }
    return 0;
}

