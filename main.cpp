#include <iostream>
#include <SFML/Graphics.hpp>

class Obstacle {
public:
    sf::RectangleShape shape;
    sf::Texture texture;

    Obstacle(sf::Vector2f size, sf::Vector2f position, const std::string& texturePath) {
        shape.setSize(size);
        shape.setPosition(position);
        if (!texture.loadFromFile(texturePath)) {
            std::cerr << "Error loading texture: " << texturePath << std::endl;
        } else {
            shape.setTexture(&texture);
        }
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

class Player {
public:
    Player() {}

    Player(const std::string& imgDirectory) {
        if (!pTexture.loadFromFile(imgDirectory)) {
            std::cerr << "Error loading image\n";
        }
        pSprite.setTexture(pTexture);
    }

    void drawPlayer(sf::RenderWindow& window) {
        window.draw(pSprite);
    }

    void movePlayer(char direction, float speed) {
        if (direction == 'u') {
            pSprite.move(0, -speed);
        }
        else if (direction == 'd') {
            pSprite.move(0, speed);
        }
        else if (direction == 'l') {
            pSprite.move(-speed, 0);
        }
        else if (direction == 'r') {
            pSprite.move(speed, 0);
        }
    }

    sf::Sprite getPlayerSprite() const {
        return pSprite;
    }

private:
    sf::Texture pTexture;
    sf::Sprite pSprite;
};

Player player("player.png");

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
    Obstacle wallUp(sf::Vector2f(800, 40), sf::Vector2f(0, 0), "Sciana800.png");
    Obstacle wallBottom(sf::Vector2f(800, 40), sf::Vector2f(0, 560), "Sciana800dol.png");
    Obstacle wallLeft(sf::Vector2f(40, 600), sf::Vector2f(0, 0), "Sciana40x600.png");
    Obstacle wallRight(sf::Vector2f(40, 600), sf::Vector2f(760, 0), "Sciana40x600prawo.png");
    Obstacle trap(sf::Vector2f(50, 50), sf::Vector2f(400, 300), "#");

    // Camera following player
    sf::View camera(sf::FloatRect(0, 0, 800, 600));
    camera.setCenter(player.getPlayerSprite().getPosition());
    window.setView(camera);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        //Controlls
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
            player.movePlayer('u', 0.05);
        }else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
            player.movePlayer('d', 0.05);
        }else if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
            player.movePlayer('l', 0.05);
        }else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
            player.movePlayer('r', 0.05);
        }

        camera.setCenter(player.getPlayerSprite().getPosition());
        window.setView(camera);

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
        wallUp.draw(window);
        wallBottom.draw(window);
        wallLeft.draw(window);
        wallRight.draw(window);
        trap.draw(window);

        // Draw player
        player.drawPlayer(window);

        // Display window
        window.display();
    }

    return 0;
}
