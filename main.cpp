#include <iostream>
#include <SFML/Graphics.hpp>
#include <random>
#include <vector>

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
        float speed = 150.0f;
        sf::Vector2f movement(0.0f, 0.0f);
        bool isMovingright = false;
        bool isMovingleft = false;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
            movement.y -= speed;
            isMovingright = true;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
            movement.y += speed;
            isMovingright = true;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
            movement.x -= speed;
            isMovingleft = true;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
            movement.x += speed;
            isMovingright = true;
        }

        position += movement * deltaTime;
        sprite.setPosition(position);

        if (isMovingright) {
            // Update animation frame
            currentFrameTime += deltaTime;
            if (currentFrameTime >= frameDuration) {
                currentFrame++;
                if (currentFrame >= animationFrames.size()) {
                    currentFrame = 0;
                }
                currentFrameTime = 0.0f;
            }
            sprite.setTextureRect(animationFrames[currentFrame]);
        }
        else if (isMovingleft) {
            // Update animation frame
            currentFrameTime += deltaTime;
            if (currentFrameTime >= frameDuration) {
                currentFrame++;
                if (currentFrame >= animationFramesleft.size()) {
                    currentFrame = 0;
                }
                currentFrameTime = 0.0f;
            }
            sprite.setTextureRect(animationFramesleft[currentFrame]);
        }
        else {
            // Player is idle
            currentFrameTime += deltaTime;
            if (currentFrameTime >= frameDuration) {
                currentFrame++;
                if (currentFrame >= idleFrames.size()) {
                    currentFrame = 0;
                }
                currentFrameTime = 0.0f;
            }
            sprite.setTextureRect(idleFrames[currentFrame]);
        }
    }


    bool hasKnife() const {
        return hasKnifeFlag;
    }

    void setHasKnife(bool hasKnife) {
        hasKnifeFlag = hasKnife;
    }

    void addAnimationFrame(const sf::IntRect& frame) {
        animationFrames.push_back(frame);
    }

    void addIdleFrame(const sf::IntRect& frame) {
        idleFrames.push_back(frame);
    }

    void addAnimationFrameleft(const sf::IntRect& frame) {
        animationFramesleft.push_back(frame);
    }

private:
    bool hasKnifeFlag = false;
    std::vector<sf::IntRect> animationFrames;
    std::vector<sf::IntRect> idleFrames;
    std::vector<sf::IntRect> animationFramesleft;
    unsigned int currentFrame = 0;
    float currentFrameTime = 0.0f;
    float frameDuration = 0.2f;
};

class Monster : public Entity {
public:
    Monster(const std::string& texturePath, const sf::Vector2f& position, float speed)
        : Entity(texturePath, position), speed(speed) {
        sprite.setScale(0.2f, 0.2f);
    }

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

        player->sprite.setScale(2.5f, 2.5f);

        player->addIdleFrame(sf::IntRect(0, 0, 50, 50));
        player->addIdleFrame(sf::IntRect(50, 0, 50, 50));
        player->addIdleFrame(sf::IntRect(100, 0, 50, 50));

        player->addAnimationFrame(sf::IntRect(150, 0, 50, 50));
        player->addAnimationFrame(sf::IntRect(200, 0, 50, 50));
        player->addAnimationFrame(sf::IntRect(250, 0, 50, 50));
        player->addAnimationFrame(sf::IntRect(300, 0, 50, 50));
        player->addAnimationFrame(sf::IntRect(350, 0, 50, 50));
        player->addAnimationFrame(sf::IntRect(400, 0, 50, 50));

        player->addAnimationFrameleft(sf::IntRect(450, 0, 50, 50));
        player->addAnimationFrameleft(sf::IntRect(500, 0, 50, 50));
        player->addAnimationFrameleft(sf::IntRect(550, 0, 50, 50));
        player->addAnimationFrameleft(sf::IntRect(600, 0, 50, 50));
        player->addAnimationFrameleft(sf::IntRect(650, 0, 50, 50));
        player->addAnimationFrameleft(sf::IntRect(700, 0, 50, 50));

        // Create monsters
        for (int i = 0; i < 3; ++i) {
            float xPos = static_cast<float>(rand() % 600 + 100);
            float yPos = static_cast<float>(rand() % 400 + 100);
            monsters.push_back(std::make_unique<Monster>("monster.png", sf::Vector2f(xPos, yPos), 80.0f));
        }

        // Create obstacles
        obstacles.push_back(std::make_unique<Obstacle>("Sciana800.png", sf::Vector2f(800, 40), sf::Vector2f(0, 0)));
        obstacles.push_back(std::make_unique<Obstacle>("Sciana800dol.png", sf::Vector2f(800, 40), sf::Vector2f(0, 560)));
        obstacles.push_back(std::make_unique<Obstacle>("Sciana40x600.png", sf::Vector2f(40, 600), sf::Vector2f(0, 0)));
        obstacles.push_back(std::make_unique<Obstacle>("Sciana40x600prawo.png", sf::Vector2f(40, 600), sf::Vector2f(760, 0)));
        obstacles.push_back(std::make_unique<Obstacle>("floor.png", sf::Vector2f(720, 520), sf::Vector2f(40, 40)));

        // Create coins
        coins.push_back(std::make_unique<Coin>(sf::Vector2f(300.0f, 200.0f)));
        coins.push_back(std::make_unique<Coin>(sf::Vector2f(600.0f, 400.0f)));

        // Load font
        if (!font.loadFromFile("arial.ttf")) {
            std::cerr << "Error loading font: arial.ttf" << std::endl;
        }

        // Create coin count text
        coinCountText.setFont(font);
        coinCountText.setCharacterSize(20);
        coinCountText.setFillColor(sf::Color::White);
        coinCountText.setPosition(680.f, 10.f);

        // Create knife
        knife = std::make_unique<Knife>(sf::Vector2f(100.0f, 100.0f));
    }
    void update(float deltaTime) {
        player->update(deltaTime);

        // Count collected coins
        int collectedCoinCount = 0;
        for (const auto& coin : coins) {
            if (coin->collected) {
                collectedCoinCount++;
            }
        }

        // Update coin count text
        coinCountText.setString("Coins: " + std::to_string(collectedCoinCount));


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
                    //std::cout << "Game Over!" << std::endl;
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

    //   void setVisibility(bool visible) {
    //       this->visible = visible;
    //   }
private:
    sf::RenderWindow window;
    std::unique_ptr<Player> player;
    std::vector<std::unique_ptr<Monster>> monsters;
    std::vector<std::unique_ptr<Obstacle>> obstacles;
    std::vector<std::unique_ptr<Coin>> coins;
    std::unique_ptr<Knife> knife;
    sf::Font font;
    sf::Text coinCountText;

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

        for (auto& monster : monsters) {
            window.draw(monster->sprite);
        }

        window.draw(player->sprite);

        // Draw coin count text
        window.draw(coinCountText);

        window.display();
    }
};

int main() {
    srand(static_cast<unsigned>(time(nullptr)));

    // Load font
    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) {
        std::cerr << "Error loading font." << std::endl;
        return 1;
    }



    Game game;
    game.run();

    return 0;
}
