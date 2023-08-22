#pragma once
#include "AnimatedRect.h"

class EnemyBullet {
public:
    AnimatedRect bulletRect;
    float speedx = 0;
    float speedy = 0;
    sf::Vector2f gravity;

    int animation_stage = 0;
    float animation_time = 0.0f;

    bool freshlycreated = true;

    EnemyBullet(sf::Vector2f pos, sf::Vector2f spriteScreenSize, sf::Vector2f spriteImageSize, GameSounds& gameSounds, sf::Vector2f _gravity = sf::Vector2f(0, 0)) {
        gravity = _gravity;
        pos = pos + sf::Vector2f(0.25f, 0.13f);
        bulletRect.Create(pos, sf::Vector2f(32, 32), spriteScreenSize, spriteImageSize, 1, 3, 0.1f);
    }

    void Step(bool& playerDead, sf::Vector2f playerPos, std::vector<std::vector<bool>>& collisionRects, sf::RenderTarget& target, sf::Texture& texture, sf::Vector2f cameraPos, float FrameElapsed, float TotalElapsed, GameSounds& gameSounds) {
        bool visible = isVisible(target, bulletRect, cameraPos);

        if (speedx == 0.0f && speedy == 0.0f) {
            bulletRect.startFrame = animation_stage;
            animation_time += FrameElapsed;
            animation_stage = (animation_time * 30.0f) + 1;
        }
        else {
            speedx += (gravity.x * FrameElapsed);
            speedy += (gravity.y * FrameElapsed);
            bulletRect.pos.x += (speedx * FrameElapsed);
            bulletRect.pos.y += (speedy * FrameElapsed);

            CollisionResult result = CollisionSolver(bulletRect.pos, sf::Vector2f(speedx, speedy), 0.25f, 0.25f, 0.2f, 0.4f, FrameElapsed, collisionRects);

            if (result.LeftCollision || result.RightCollision) {
                speedx = 0.0f;
                speedy = 0.0f;
                if (visible) {
                    gameSounds.playSound("shotHit", 50.0f);
                }
            }
            if (result.TopCollision || result.BottomCollision) {
                speedx = 0.0f;
                speedy = 0.0f;
                if (visible) {
                    gameSounds.playSound("shotHit", 50.0f);
                }
            }

            // playerpos
            if (PointCollisionSolver(playerPos, bulletRect.pos, -0.9f, 0.5f, -0.6f, 0.6f)) {
                playerDead = true;
            }
        }

        if (visible) {
            if (freshlycreated) {
                gameSounds.playSound("shoot", 50.0f);
            }
            bulletRect.draw(target, texture, cameraPos, TotalElapsed, 1, {1, 3});
        }

        freshlycreated = false;
    }
};