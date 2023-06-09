
#include <iostream>
#include <SFML/Graphics.hpp>

class Obstacle {
public:
    sf::RectangleShape shape;

    Obstacle(sf::Vector2f size, sf::Vector2f position) {
        shape.setSize(size);
        shape.setPosition(position);
        shape.setFillColor(sf::Color::Red);
    }

    void draw(sf::RenderWindow& window) {
        window.draw(shape);
    }
};

class Monster {
public:
    sf::Sprite sprite;
    sf::Texture texture;

    Monster(sf::Texture& tex, sf::Vector2f position) {
        texture = tex;
        sprite.setTexture(texture);
        sprite.setPosition(position);
    }

    void draw(sf::RenderWindow& window) {
        window.draw(sprite);
    }
};

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "SFML Monster Game");

    // Load monster images from dictionary
    sf::Texture monsterTexture1, monsterTexture2, monsterTexture3;
    if (!monsterTexture1.loadFromFile("monster1.png") ||
        !monsterTexture2.loadFromFile("monster2.png") ||
        !monsterTexture3.loadFromFile("monster3.png")) {
        // Handle failed loading
        return 1;
    }

    // Create monsters
    Monster monster1(monsterTexture1, sf::Vector2f(100, 100));
    Monster monster2(monsterTexture2, sf::Vector2f(300, 200));
    Monster monster3(monsterTexture3, sf::Vector2f(500, 300));

    // Create obstacles
    Obstacle wall(sf::Vector2f(50, 200), sf::Vector2f(200, 100));
    Obstacle trap(sf::Vector2f(50, 50), sf::Vector2f(400, 300));

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // Check collision with trap
        if (monster1.sprite.getGlobalBounds().intersects(trap.shape.getGlobalBounds())) {
            // Monster 1 touched the trap, make it disappear
            monster1.sprite.setPosition(-100, -100); // Move it out of the screen
        }

        // Clearing window
        window.clear();

        // Drawing  monsters and obstacles in window
        monster1.draw(window);
        monster2.draw(window);
        monster3.draw(window);
        wall.draw(window);
        trap.draw(window);

        // Display window
        window.display();
    }

    return 0;
}
