#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <cstdlib>
#include <ctime>

using namespace sf;

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const float PLAYER_SPEED = 5.0f;
const float BULLET_SPEED = 8.0f;
const float ENEMY_SPEED = 2.0f;
const float BOSS_SPEED = 1.5f;

class Bullet {
public:
    CircleShape shape;
    Bullet(float x, float y) {
        shape.setRadius(5);
        shape.setFillColor(Color::Red);
        shape.setPosition(x, y);
    }
    void update() { shape.move(0, -BULLET_SPEED); }
};

class Enemy {
public:
    RectangleShape shape;
    Enemy(float x, float y) {
        shape.setSize(Vector2f(40, 40));
        shape.setFillColor(Color::Green);
        shape.setPosition(x, y);
    }
    void update() { shape.move(0, ENEMY_SPEED); }
};

class Boss {
public:
    RectangleShape shape;
    int health = 10;
    Boss(float x, float y) {
        shape.setSize(Vector2f(100, 100));
        shape.setFillColor(Color::Magenta);
        shape.setPosition(x, y);
    }
    void update() {
        shape.move(BOSS_SPEED * ((rand() % 2 == 0) ? -1 : 1), 0);
        if (shape.getPosition().x < 0 || shape.getPosition().x > WINDOW_WIDTH - 100) {
            BOSS_SPEED = -BOSS_SPEED;
        }
    }
};

int main() {
    srand(time(0));
    RenderWindow window(VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Shooter Game");

    Texture bgTexture;
    bgTexture.loadFromFile("background.png");
    Sprite background(bgTexture);
    background.setScale(1.0f, 1.0f);

    RectangleShape player(Vector2f(50, 50));
    player.setFillColor(Color::Blue);
    player.setPosition(WINDOW_WIDTH / 2, WINDOW_HEIGHT - 70);

    std::vector<Bullet> bullets;
    std::vector<Enemy> enemies;
    Boss boss(WINDOW_WIDTH / 2, 50);

    Clock enemySpawnClock, bossMoveClock;
    SoundBuffer shootBuffer;
    shootBuffer.loadFromFile("shoot.wav");
    Sound shootSound(shootBuffer);

    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed) {
                window.close();
            }
        }
        
        if (Keyboard::isKeyPressed(Keyboard::Left) && player.getPosition().x > 0) {
            player.move(-PLAYER_SPEED, 0);
        }
        if (Keyboard::isKeyPressed(Keyboard::Right) && player.getPosition().x < WINDOW_WIDTH - 50) {
            player.move(PLAYER_SPEED, 0);
        }
        if (Keyboard::isKeyPressed(Keyboard::Space)) {
            bullets.push_back(Bullet(player.getPosition().x + 20, player.getPosition().y));
            shootSound.play();
        }
        
        for (auto &bullet : bullets) bullet.update();
        bullets.erase(std::remove_if(bullets.begin(), bullets.end(), [](Bullet &b) { return b.shape.getPosition().y < 0; }), bullets.end());
        
        if (enemySpawnClock.getElapsedTime().asSeconds() > 1.0f) {
            enemies.push_back(Enemy(rand() % (WINDOW_WIDTH - 40), 0));
            enemySpawnClock.restart();
        }
        for (auto &enemy : enemies) enemy.update();
        enemies.erase(std::remove_if(enemies.begin(), enemies.end(), [](Enemy &e) { return e.shape.getPosition().y > WINDOW_HEIGHT; }), enemies.end());
        
        if (bossMoveClock.getElapsedTime().asSeconds() > 0.5f) {
            boss.update();
            bossMoveClock.restart();
        }
        
        window.clear();
        window.draw(background);
        window.draw(player);
        for (auto &bullet : bullets) window.draw(bullet.shape);
        for (auto &enemy : enemies) window.draw(enemy.shape);
        window.draw(boss.shape);
        window.display();
    }
    return 0;
}
