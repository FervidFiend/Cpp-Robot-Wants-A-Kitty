#pragma once
#include "gameMap.h"
#include "HelperFunctions.h"
#include "SoundHelper.h"
#include "Bullet.h"

class Enemy {
public:
    AnimatedRect enemyRect;
    sf::Texture* texture;
    sf::Vector2f moveSpeed;
    sf::Vector2f velocity;
    int health;
    int max_health;

    int mode;
    std::vector<int> frameSequence;

    float ratio;

    float colOffsetX = 0.0f;
    float colOffsetY = 0.0f;

    float colOffsetleft = 0.0f;
    float colOffsetright = 0.0f;
    float colOffsetup = 0.0f;
    float colOffsetdown = 0.0f;

    std::string type;




    float timeSinceLastShot = 0.0f;







    Enemy() {

    }

    Enemy(sf::Vector2f pos, sf::Texture* _texture, sf::Vector2f _moveSpeed, int _max_health, std::string _type, sf::Vector2f _sizeInWorld = sf::Vector2f(32, 32), sf::Vector2f _sizeOnImage = sf::Vector2f(16, 16), int _mode = 0, std::vector<int> _frameSequence = {}) {
        texture = _texture;
        moveSpeed = _moveSpeed;
        velocity = _moveSpeed;
        max_health = _max_health;
        health = max_health;
        type = _type;
        mode = _mode;
        frameSequence = _frameSequence;

        enemyRect.Create(pos, sf::Vector2f(32, 32), _sizeInWorld, _sizeOnImage, 0, 4, 0.25f);
        enemyRect.facingLeft = rand() % 2;
        
        ratio = getRectRatio(enemyRect);
    }

    void Step(sf::RenderTarget& target, sf::Vector2f cameraPos, float FrameElapsed, float TotalElapsed, GameSounds& gameSounds, std::vector<std::vector<bool>>& collisionRects, std::vector<EnemyBullet>& enemyBullets) {
        float newPosX = enemyRect.pos.x + velocity.x * FrameElapsed;
        float newPosY = enemyRect.pos.y + velocity.y * FrameElapsed;

        CollisionResult colresult = CollisionSolver(enemyRect.pos, velocity, 0.0f, ratio, 0.0f, ratio, FrameElapsed, collisionRects);

        if (colresult.LeftCollision || colresult.RightCollision) {
            velocity.x *= -1;
        }
        if (colresult.BottomCollision || colresult.TopCollision) {
            velocity.y *= -1;
            enemyRect.facingLeft = rand() % 2;
        }

        if (velocity.y == 0) {
            CollisionResult willFallLeft = CollisionSolver(sf::Vector2f(enemyRect.pos.x, enemyRect.pos.y), velocity, 0.1f, 0.0f, 0.6f, ratio + 0.5f, FrameElapsed, collisionRects, true);

            CollisionResult willFallRight = CollisionSolver(sf::Vector2f(enemyRect.pos.x + 0.9f, enemyRect.pos.y), velocity, 0.0f, 0.0f, 0.6f, ratio + 0.5f, FrameElapsed, collisionRects, true);

            if (!willFallLeft.BottomCollision) {
                velocity.x = moveSpeed.x;
            }
            else if (!willFallRight.BottomCollision) {
                velocity.x = -moveSpeed.x;
            }
        }

        if (health <= 0) {
            gameSounds.playSound("alienDie", 50.0f);
        }
        else if (type == "drip") {
            timeSinceLastShot += FrameElapsed;
            if (timeSinceLastShot >= 1.0f) {
                EnemyBullet newBullet(enemyRect.pos, sf::Vector2f(16, 16), sf::Vector2f(8, 8), gameSounds, sf::Vector2f(0, 1));
                newBullet.speedy = 10.0f;
                newBullet.bulletRect.pos.y += 0.2f;
                enemyBullets.push_back(newBullet);

                timeSinceLastShot = 0.0f;
            }
        }
        else if (type == "boss") {
            float health_mult = float(health) / float(max_health);
            if (velocity.x > 0) {
                velocity.x = 1.0f + ((1.0f - health_mult) * 10);
            }
            else if (velocity.x < 0) {
                velocity.x = -(1.0f + ((1.0f - health_mult) * 10));
            }

            timeSinceLastShot += FrameElapsed;
            if (timeSinceLastShot >= (health_mult * 0.95f) + 0.05f) {
                EnemyBullet newBullet(enemyRect.pos, sf::Vector2f(16, 16), sf::Vector2f(8, 8), gameSounds, sf::Vector2f(0, 22));
                newBullet.speedy = -10.0f;
                newBullet.bulletRect.pos.x += (ratio / 2.0f);
                newBullet.bulletRect.pos.y += (ratio / 2.0f);

                newBullet.speedx = -10.0f;
                enemyBullets.push_back(newBullet);

                newBullet.speedx = 10.0f;
                enemyBullets.push_back(newBullet);

                timeSinceLastShot = 0.0f;
            }
        }

        enemyRect.pos.x = newPosX;
        enemyRect.pos.y = newPosY;

        if (isVisible(target, enemyRect, cameraPos)) {
            if (velocity.x > 0) {
                enemyRect.facingLeft = false;
            }
            else if (velocity.x < 0) {
                enemyRect.facingLeft = true;
            }

            enemyRect.draw(target, *texture, cameraPos, TotalElapsed, mode, frameSequence);
        }
    }
};