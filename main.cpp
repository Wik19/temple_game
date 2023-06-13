#include <iostream>
#include <SFML/Graphics.hpp>
#include <random>

class Entity {
public:
    sf::Sprite sprite;

    Entity(const std::string& texturePath, const sf::Vector2f& position)
        : position(position)
    {
        if (!texture.loadFromFile(texturePath)) {
            std::cerr << "Error loading texture: " << texturePath << std::endl;
        }
        else {
            sprite.setTexture(texture);
            sprite.setPosition(position);
        }
    }

    virtual void update(float deltaTime) {}
    virtual void draw(sf::RenderWindow& window) {
        window.draw(sprite);
    }
    virtual sf::FloatRect getGlobalBounds() const {
        return sprite.getGlobalBounds();
    }
    sf::Texture& getTexture() {
        return texture;
    }

protected:
    sf::Texture texture;
    sf::Vector2f position;
};

class Player : public Entity {
public:
    Player(const std::string& texturePath, const sf::Vector2f& position)
        : Entity(texturePath, position) {}

    void update(float deltaTime) override {
        float speed = 100.0f;
        sf::Vector2f movement(0.0f, 0.0f);

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
            movement.y -= speed;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
            movement.y += speed;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
            movement.x -= speed;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
            movement.x += speed;
        }

        position += movement * deltaTime;
        sprite.setPosition(position);
    }

    bool hasKnife() const {
        return hasKnifeFlag;
    }

    void setHasKnife(bool hasKnife) {
        hasKnifeFlag = hasKnife;
    }

private:
    bool hasKnifeFlag = false;
};

class Monster : public Entity {
public:
    Monster(const std::string& texturePath, const sf::Vector2f& position, float speed)
        : Entity(texturePath, position), speed(speed) {}

    void update(float deltaTime) override {
        // Move towards the player
        sf::Vector2f direction = playerPosition - position;
        float distance = std::sqrt(direction.x * direction.x + direction.y * direction.y);

        if (distance > 0.0f) {
            direction /= distance;
            position += direction * speed * deltaTime;
            sprite.setPosition(position);
        }
    }

    void setPlayerPosition(const sf::Vector2f& playerPos) {
        playerPosition = playerPos;
    }

    bool isCollidingWithPlayer(const sf::FloatRect& playerBounds) const {
        return sprite.getGlobalBounds().intersects(playerBounds);
    }

private:
    float speed;
    sf::Vector2f playerPosition;
};

class Obstacle : public Entity {
public:
    Obstacle(const std::string& texturePath, const sf::Vector2f& size, const sf::Vector2f& position)
        : Entity(texturePath, position)
    {
        sprite.setScale(size.x / sprite.getLocalBounds().width, size.y / sprite.getLocalBounds().height);
    }
};

class Coin : public Entity {
public:
    bool collected = false;

    Coin(const sf::Vector2f& position)
        : Entity("coin.png", position) {}

    void update(float deltaTime) override {
        // Spin the coin
        sprite.rotate(100.0f * deltaTime);
    }
};
class Knife : public Entity {
public:
    bool collected = false;

    Knife(const sf::Vector2f& position)
        : Entity("knife.png", position) {}
};

class Game {
public:
    Game() : window(sf::VideoMode(800, 600), "Game") {
        // Create player
        player = std::make_unique<Player>("player.png", sf::Vector2f(400.0f, 300.0f));

        // Create monsters
        for (int i = 0; i < 3; ++i) {
            float xPos = static_cast<float>(rand() % 600 + 100);
            float yPos = static_cast<float>(rand() % 400 + 100);
            monsters.push_back(std::make_unique<Monster>("monster.png", sf::Vector2f(xPos, yPos), 80.0f));
        }

        // Create obstacles
        obstacles.push_back(std::make_unique<Obstacle>("obstacle.png", sf::Vector2f(100.0f, 200.0f), sf::Vector2f(200.0f, 100.0f)));
        obstacles.push_back(std::make_unique<Obstacle>("obstacle.png", sf::Vector2f(200.0f, 100.0f), sf::Vector2f(500.0f, 400.0f)));

        // Create coins
        coins.push_back(std::make_unique<Coin>(sf::Vector2f(300.0f, 200.0f)));
        coins.push_back(std::make_unique<Coin>(sf::Vector2f(600.0f, 400.0f)));

        // Create knife
        knife = std::make_unique<Knife>(sf::Vector2f(100.0f, 100.0f));
    }
    void update(float deltaTime) {
        player->update(deltaTime);

        // Check collision between player and coins
        for (auto& coin : coins) {
            if (!coin->collected && player->sprite.getGlobalBounds().intersects(coin->sprite.getGlobalBounds())) {
                coin->collected = true;
                coin->sprite.setPosition(-1000.f, -1000.f); // Move the coin offscreen
                // Give some points or perform other actions when the player collects a coin
            }
        }

        // Check collision between player and knife
        if (!knife->collected && player->sprite.getGlobalBounds().intersects(knife->sprite.getGlobalBounds())) {
            knife->collected = true;
            knife->sprite.setPosition(-1000.f, -1000.f); // Move the knife offscreen
            // Give the player the ability to kill monsters
            player->setHasKnife(true);
        }

        // Check collision between player and monsters
        sf::FloatRect playerBounds = player->getGlobalBounds();
        for (auto it = monsters.begin(); it != monsters.end(); ) {
            if ((*it)->isCollidingWithPlayer(playerBounds)) {
                if (player->hasKnife()) {
                    it = monsters.erase(it); // Remove the monster from the vector
                    continue;
                } else {
                    // Game over condition when player collides with a monster without the knife
                    std::cout << "Game Over!" << std::endl;
                    break;
                }
            }
            ++it;
        }

        for (auto& monster : monsters) {
            monster->setPlayerPosition(player->sprite.getPosition());
            monster->update(deltaTime);
        }

        for (auto& coin : coins) {
            coin->update(deltaTime);
        }
        sprite.setTexture(texture);
        sprite.setPosition(position);
        visible = false;
    }
    void run() {
        sf::Clock clock;

        while (window.isOpen()) {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    window.close();
                }
            }

            float deltaTime = clock.restart().asSeconds();

            update(deltaTime);
            render();
        }
    }

    void setVisibility(bool visible) {
        this->visible = visible;
    }

    void addHeart() {
        sf::Sprite heart;
        heart.setTexture(texture);
        float x = sprite.getPosition().x + (hearts.size() * sprite.getGlobalBounds().width);
        float y = sprite.getPosition().y;
        heart.setPosition(sf::Vector2f(x, y));
        hearts.push_back(heart);
    }

private:
    sf::RenderWindow window;
    std::unique_ptr<Player> player;
    std::vector<std::unique_ptr<Monster>> monsters;
    std::vector<std::unique_ptr<Obstacle>> obstacles;
    std::vector<std::unique_ptr<Coin>> coins;
    std::unique_ptr<Knife> knife;

    void update1(float deltaTime) {
        player->update(deltaTime);

        // Check collision between player and coins
        for (auto& coin : coins) {
            if (!coin->collected && player->sprite.getGlobalBounds().intersects(coin->sprite.getGlobalBounds())) {
                coin->collected = true;
                coin->sprite.setPosition(-1000.f, -1000.f); // Move the coin offscreen
                // Give the player the knife when they collect the coin
                player->setHasKnife(true);
                knife->collected = true;
                knife->sprite.setPosition(-1000.f, -1000.f); // Move the knife offscreen
            }
        }

        // Check collision between player and monsters
        sf::FloatRect playerBounds = player->getGlobalBounds();
        for (auto it = monsters.begin(); it != monsters.end(); ) {
            if ((*it)->isCollidingWithPlayer(playerBounds)) {
                if (player->hasKnife()) {
                    it = monsters.erase(it); // Remove the monster from the vector
                    continue;
                }
                else {
                    // Game over condition when player collides with a monster without the knife
                    std::cout << "Game Over!" << std::endl;
                    break;
                }
            }
            ++it;
        }

        for (auto& monster : monsters) {
            monster->setPlayerPosition(player->sprite.getPosition());
            monster->update(deltaTime);
        }

        for (auto& coin : coins) {
            coin->update(deltaTime);
        }
    }

    void render() {
        window.clear();

        window.draw(player->sprite);

        for (auto& monster : monsters) {
            window.draw(monster->sprite);
        }

        for (auto& obstacle : obstacles) {
            window.draw(obstacle->sprite);
        }

        for (auto& coin : coins) {
            if (!coin->collected) {
                window.draw(coin->sprite);
            }
        }

        if (!knife->collected) {
            window.draw(knife->sprite);
        }

        window.display();
    }
};

int main() {
    srand(static_cast<unsigned>(time(nullptr)));

    Game game;
    game.run();

    return 0;
}
