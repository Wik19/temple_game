#include <iostream>
#include <SFML/Graphics.hpp>
#include <random>

class Obstacle {
public:
    sf::RectangleShape shape;
    sf::Texture texture;

    Obstacle(sf::Vector2f size, sf::Vector2f position, const std::string& texturePath) {
        shape.setSize(size);
        shape.setPosition(position);
        if (!texture.loadFromFile(texturePath)) {
            std::cerr << "Error loading texture: " << texturePath << std::endl;
        }
        else {
            shape.setTexture(&texture);
            textureSize = sf::Vector2f(texture.getSize());
        }
    }

    void draw(sf::RenderWindow& window) {
        shape.setTextureRect(sf::IntRect(0, 0, shape.getSize().x, shape.getSize().y));
        window.draw(shape);
    }

    sf::FloatRect getGlobalBounds() const {
        return shape.getGlobalBounds();
    }

private:
    sf::Vector2f textureSize;
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

    sf::FloatRect getGlobalBounds() const {
        return sprite.getGlobalBounds();
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

    sf::FloatRect getGlobalBounds() const {
        return pSprite.getGlobalBounds();
    }

private:
    sf::Texture pTexture;
    sf::Sprite pSprite;
};

class Coin {
public:
    sf::Sprite sprite;
    sf::Texture texture;
    bool isCollected;

    Coin(sf::Vector2f position) {
        if (!texture.loadFromFile("coin.png")) {
            std::cerr << "Error loading coin texture.\n";
        }
        sprite.setTexture(texture);
        sprite.setPosition(position);
        isCollected = false;
    }

    void draw(sf::RenderWindow& window) {
        if (!isCollected) {
            window.draw(sprite);
        }
    }

    void setPosition(const sf::Vector2f& position) {
        sprite.setPosition(position);
    }

    sf::FloatRect getGlobalBounds() const {
        return sprite.getGlobalBounds();
    }
};

class Knife {
public:
    sf::Sprite sprite;
    sf::Texture texture;
    bool isCollected;

    Knife(sf::Vector2f position) {
        if (!texture.loadFromFile("knife.png")) {
            std::cerr << "Error loading knife texture.\n";
        }
        sprite.setTexture(texture);
        sprite.setPosition(position);
        isCollected = false;
    }

    void draw(sf::RenderWindow& window) {
        if (!isCollected) {
            window.draw(sprite);
        }
    }

    void setPosition(const sf::Vector2f& position) {
        sprite.setPosition(position);
    }

    sf::FloatRect getGlobalBounds() const {
        return sprite.getGlobalBounds();
    }
};

class InventoryMenu {
public:
    InventoryMenu() {
        // Load weapon and perk textures
        if (!weaponTexture1.loadFromFile("weapon1.png") ||
            !weaponTexture2.loadFromFile("weapon2.png") ||
            !weaponTexture3.loadFromFile("weapon3.png") ||
            !perkTexture.loadFromFile("perk.png")) {
            std::cerr << "Error loading inventory item textures.\n";
        }

        // Set the position and size of the inventory menu
        const float menuWidth = 120.0f;
        const float menuHeight = 40.0f;
        const float centerX = 800.0f - menuWidth / 2 - 10.0f;
        const float centerY = 600.0f - menuHeight / 2 - 10.0f;

        // Create perk circle and set its position
        sf::CircleShape perkCircle(menuHeight / 2);
        perkCircle.setTexture(&perkTexture);
        perkCircle.setPosition(centerX, centerY);
        perkCircle.setOrigin(menuHeight / 2, menuHeight / 2);
        perkCircles.push_back(perkCircle);

        // Calculate the position and size of the weapon circles
        const float weaponSize = menuHeight * 0.6f;
        const float weaponSpacing = (menuWidth - 2 * weaponSize) / 3;

        // Create weapon circles and set their positions
        for (int i = 0; i < numWeapons; ++i) {
            sf::CircleShape weaponCircle(weaponSize / 2);
            weaponCircle.setTexture(&getWeaponTexture(i + 1));
            float x = centerX - menuWidth / 2 + weaponSize / 2 + (weaponSize + weaponSpacing) * i;
            float y = centerY;
            weaponCircle.setPosition(x, y);
            weaponCircle.setOrigin(weaponSize / 2, weaponSize / 2);
            weaponCircles.push_back(weaponCircle);
        }
    }

    void draw(sf::RenderWindow& window) {
        for (const auto& perkCircle : perkCircles) {
            window.draw(perkCircle);
        }
        for (const auto& weaponCircle : weaponCircles) {
            window.draw(weaponCircle);
        }
    }

private:
    static constexpr int numWeapons = 3;  // Number of weapons in the inventory

    sf::Texture weaponTexture1;  // Texture for weapon 1
    sf::Texture weaponTexture2;  // Texture for weapon 2
    sf::Texture weaponTexture3;  // Texture for weapon 3
    sf::Texture perkTexture;     // Texture for perk

    std::vector<sf::CircleShape> weaponCircles;  // Circles for weapons
    std::vector<sf::CircleShape> perkCircles;    // Circles for perk

    sf::Texture& getWeaponTexture(int weaponIndex) {
        switch (weaponIndex) {
        case 1:
            return weaponTexture1;
        case 2:
            return weaponTexture2;
        case 3:
            return weaponTexture3;
        default:
            throw std::out_of_range("Invalid weapon index");
        }
    }
};

Player player("player.png");
int coinCount = 0;

void handleCollision(Player& player, Coin& coin) {
    if (!coin.isCollected && player.getGlobalBounds().intersects(coin.getGlobalBounds())) {
        coin.isCollected = true;
        coinCount++;
        std::cout << "Coins collected: " << coinCount << std::endl;
    }
}

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
    Obstacle floor(sf::Vector2f(720, 520), sf::Vector2f(40, 40), "floor.png");
    Obstacle trap(sf::Vector2f(50, 50), sf::Vector2f(400, 300), "#");

    // Create coin
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> disX(40.0f, 760.0f); // Random X position within the playable area
    std::uniform_real_distribution<float> disY(40.0f, 560.0f); // Random Y position within the playable area

    Coin coin(sf::Vector2f(disX(gen), disY(gen)));

    InventoryMenu inventoryMenu;

    // Create knife
    Knife knife(sf::Vector2f(80.0, 500.0));

    // Create font for counter text
    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) {
        std::cerr << "Error loading font\n";
        return 1;
    }

    // Create text object for counter
    sf::Text counterText;
    counterText.setFont(font);
    counterText.setCharacterSize(24);
    counterText.setFillColor(sf::Color::White);
    counterText.setPosition(650, 10); // Updated position

    int coinCount = 0;

    bool coinCollected = false;

    bool knifeCollected = false;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // Controls
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
            // Check collision with walls
            if (!player.getGlobalBounds().intersects(wallUp.getGlobalBounds())) {
                player.movePlayer('u', 0.05);
                handleCollision(player, coin);
            }
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
            if (!player.getGlobalBounds().intersects(wallBottom.getGlobalBounds())) {
                player.movePlayer('d', 0.05);
                handleCollision(player, coin);
            }
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
            if (!player.getGlobalBounds().intersects(wallLeft.getGlobalBounds())) {
                player.movePlayer('l', 0.05);
                handleCollision(player, coin);
            }
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
            if (!player.getGlobalBounds().intersects(wallRight.getGlobalBounds())) {
                player.movePlayer('r', 0.05);
                handleCollision(player, coin);
            }
        }

        // Check collision with trap
        if (player.getGlobalBounds().intersects(trap.getGlobalBounds())) {
            // Player touched the trap, do something
        }

        // Check collision with coin
        if (player.getGlobalBounds().intersects(coin.getGlobalBounds()) && !coinCollected) {
            // Player collected the coin
            coinCount++;
            coinCollected = true;
            // Generate new random position for the coin
            coin.setPosition(sf::Vector2f(disX(gen), disY(gen)));
        }

        // Check collision with knife
        if (player.getGlobalBounds().intersects(knife.getGlobalBounds()) && !knifeCollected) {
            // Player collected the knife
            knifeCollected = true;
        }

        // Clearing window
        window.clear();

        // Drawing monsters and obstacles in window
        monster1.draw(window);
        monster2.draw(window);
        monster3.draw(window);
        floor.draw(window);
        wallUp.draw(window);
        wallBottom.draw(window);
        wallLeft.draw(window);
        wallRight.draw(window);
        inventoryMenu.draw(window);
        // Draw coin
        coin.draw(window);

        if (!knifeCollected) {
            knife.draw(window);
        }

        // Draw player
        player.drawPlayer(window);

        trap.draw(window);

        // Draw counter text
        counterText.setString("Coins: " + std::to_string(coinCount));
        window.draw(counterText);

        // Display window
        window.display();
    }

    return 0;
}
