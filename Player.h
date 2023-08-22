#pragma once
#include "HelperFunctions.h"
#include "gameMap.h"
#include "SoundHelper.h"
#include "MyText.h"

class PlayerBullet {
public:
    AnimatedRect bulletRect;
    float speed = 10;

    int animation_stage = 0;
    float animation_time = 0.0f;

    PlayerBullet(sf::Vector2f pos, sf::Vector2f spriteScreenSize, sf::Vector2f spriteImageSize, bool left, GameSounds& gameSounds) {
        gameSounds.playSound("shoot", 100.0f);
        if (left) {
            speed *= -1.0f;
        }
        pos = pos + sf::Vector2f(0.25f, 0.13f);
        bulletRect.Create(pos, sf::Vector2f(32, 32), spriteScreenSize, spriteImageSize, 0, 1, 120.0f);
    }

    void Step(GameMap& gameMap, sf::RenderTarget& target, sf::Texture& texture, sf::Vector2f cameraPos, float FrameElapsed, float TotalElapsed, GameSounds& gameSounds) {
        if (speed == 0.0f) {
            bulletRect.startFrame = animation_stage;
            animation_time += FrameElapsed;
            animation_stage = (animation_time * 30.0f) + 1;
        }
        else {
            bulletRect.pos.x += (speed * FrameElapsed);

            CollisionResult result = CollisionSolver(bulletRect.pos, sf::Vector2f(speed, 0), 0.25f, 0.25f, 0.2f, 0.4f, FrameElapsed, gameMap.collisionRects);

            if (result.LeftCollision || result.RightCollision) {
                speed = 0.0f;
                gameSounds.playSound("shotHit", 100.0f);
            }

            for (auto& enemy : gameMap.enemys) {
                if (PointCollisionSolver(bulletRect.pos, sf::Vector2f(enemy.enemyRect.pos.x + enemy.colOffsetX, enemy.enemyRect.pos.y + enemy.colOffsetY), -0.15f, 0.9f * enemy.ratio, -0.4f, 1.0f * enemy.ratio)) {
                    enemy.health -= 1;
                    if (enemy.type == "boss") {
                        gameSounds.playSound("alienBossHit", 100.0f);
                    }
                    else {
                        enemy.velocity.x *= -1;
                        gameSounds.playSound("alienHit", 100.0f);
                    }
                    speed = 0.0f;
                    break;
                }
            }
        }

        if (isVisible(target, bulletRect, cameraPos)) {
            bulletRect.draw(target, texture, cameraPos, TotalElapsed);
        }
    }
};

class Missile {
public:
    AnimatedRect missileRect;
    float speed = -10.0f;

    int animation_stage = 0;
    float animation_time = 0.0f;

    float accel = -10.0f;

    Missile(sf::Vector2f pos, sf::Vector2f spriteScreenSize, sf::Vector2f spriteImageSize, bool left, GameSounds& gameSounds) {
        gameSounds.playSound("missileShoot", 100.0f);
        pos = pos + sf::Vector2f(0.0f, -0.18f);
        missileRect.Create(pos, sf::Vector2f(32, 32), spriteScreenSize, spriteImageSize, 0, 2, 0.125f);
    }

    void Step(GameMap& gameMap, sf::RenderTarget& target, sf::Texture& texture, sf::Vector2f cameraPos, float FrameElapsed, float TotalElapsed, GameSounds& gameSounds) {
        missileRect.pos.y += (speed * FrameElapsed);

        bool result = false;

        for (auto& destructableTile : gameMap.destructableTiles) {
            if (PointCollisionSolver(destructableTile.destructableRect.pos, missileRect.pos, -1.0f, 1.0f, -1.0f, 1.0f)) {
                destructableTile.destroyed = true;
                gameMap.collisionRects[destructableTile.destructableRect.pos.y][destructableTile.destructableRect.pos.x] = false;
                result = true;
            }
        }

        CollisionResult colresult = CollisionSolver(missileRect.pos, sf::Vector2f(0, speed), 0.3f, 0.7f, 0.1f, 0.9f, FrameElapsed, gameMap.collisionRects);

        if (colresult.TopCollision || result) {
            missileRect.numberOfFrames = 1;
            speed = 0.0f;
            gameSounds.playSound("missileHit", 100.0f);
        }

        for (auto& enemy : gameMap.enemys) {
            if (PointCollisionSolver(missileRect.pos, sf::Vector2f(enemy.enemyRect.pos.x + enemy.colOffsetX, enemy.enemyRect.pos.y + enemy.colOffsetY), -0.9f, 0.9f * enemy.ratio, -0.2f, 1.0f * enemy.ratio)) {
                enemy.health -= 5;
                if (enemy.type == "boss") {
                    gameSounds.playSound("alienBossHit", 100.0f);
                }
                else {
                    enemy.velocity.x *= -1;
                    gameSounds.playSound("alienHit", 100.0f);
                }
                speed = 0.0f;
                break;
            }
        }

        if (speed == 0.0f) {
            missileRect.startFrame = animation_stage;
            animation_time += FrameElapsed;
            animation_stage = (animation_time * 30.0f) + 2;
        }
        else {
            speed += accel * FrameElapsed;
        }

        if (isVisible(target, missileRect, cameraPos)) {
            missileRect.draw(target, texture, cameraPos, TotalElapsed);
        }
    }
};

class Player {
public:
    sf::Texture bulletTexture;
    sf::Texture missileTexture;

    sf::Texture playerTexture;
    sf::Texture rocketTexture;

    AnimatedRect playerRect;
    AnimatedRect rocketRect;

    sf::Vector2f reStartPos = sf::Vector2f(0, 0);

    sf::Vector2f startPos = sf::Vector2f(0, 0);

    sf::Vector2f playerPos = sf::Vector2f(0, 0);
    sf::Vector2f cameraPos = sf::Vector2f(0, 0);

    sf::Vector2f spriteScreenSize = sf::Vector2f(32, 32);

    sf::Vector2f screenSizeInWorldSize;

    float rocketAcrossTime = 1.0f;
    float rocketUpTime = 0.5f;

    float jumpSpeed = 13.0f;
    float walkSpeed = 5.0f;

    float timeSinceLastShot = 0.0f;
    float shootingSpeed;

    float timeSinceLastMissileShot = 0.0f;
    float missileShootingSpeed = 0.5f;

    float RunningTime = 0.0f;

    bool shooting = false;
    bool shootingRocket = false;
    bool falling = true;
    bool jumpPressed = false;
    bool specialPressed = false;
    bool rocketingUp = false;
    bool boostingAcross = false;
    bool crouching = false;
    bool stationary = false;

    float rocketingCollisionOffset = 0.0f;

    float playerWidthCollisionOffset = 0.19f;
    float playerTopCollisionOffset = 0.1f;

    bool hasDoubleJumped = false;
    bool hasRocketed = false;

    float timeRocketed = 0.0f;
    float timeBoosted = 0.0f;

    bool dead = false;

    sf::Vector2f velocity = sf::Vector2f(0, 0);

    sf::Vector2f gravity = sf::Vector2f(0, 30.0f);

    std::vector<PlayerBullet> bullets;
    std::vector<Missile> missiles;

    Player() {
        bulletTexture.loadFromFile("images/LaserImage.png");
        missileTexture.loadFromFile("images/MissileImage.png");

        bulletTexture.setSmooth(false);
        missileTexture.setSmooth(false);

        playerTexture.loadFromFile("images/PlayerImage.png");
        rocketTexture.loadFromFile("images/RocketImage.png");

        playerTexture.setSmooth(false);
        rocketTexture.setSmooth(false);
    }

    void reset() {
        shootingSpeed = 0.3f;
        startPos = reStartPos;
        playerPos = startPos;
        RunningTime = 0.0f;
    }

    void Create(sf::Vector2f screenSize) {
        screenSizeInWorldSize = sf::Vector2f(screenSize.x / spriteScreenSize.x, screenSize.y / spriteScreenSize.y);

        cameraPos = screenSizeInWorldSize / 2.0f;
        cameraPos.x = cameraPos.x - 0.5f;
        cameraPos.y = (cameraPos.y - 0.5f) - (1.0f / 16.0f);

        playerRect.Create(cameraPos, sf::Vector2f(32, 32), spriteScreenSize, sf::Vector2f(16, 16), 1, 2, 0.125);
        rocketRect.Create(sf::Vector2f(cameraPos.x, cameraPos.y + 0.85f), sf::Vector2f(32, 32), spriteScreenSize, sf::Vector2f(16, 16), 0, 1, 120.0f);

        reStartPos = sf::Vector2f(10, 68);
        //reStartPos = sf::Vector2f(53, 53);
        reset();
    }

    void HandleEvents(float FrameElapsed, GameMap& gameMap, GameSounds& gameSounds) {
        shooting = false;
        shootingRocket = false;
        stationary = true;
        crouching = false;
        //velocity.x = 0.0f;

        if (falling == false) {
            if (jumpPressed && gameMap.pickups["jumpPower"].collected) {
                gameSounds.playSound("playerJump", 100.0f);
                velocity.y = -jumpSpeed;
            }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
                crouching = true;
                velocity.x = 0.0f;
            }
            hasDoubleJumped = false;
            if (!boostingAcross)
            {
                hasRocketed = false;
            }
        }
        else {
            if (jumpPressed && hasDoubleJumped == false && gameMap.pickups["jumpPower"].collected && gameMap.pickups["doubleJumpPower"].collected) {
                gameSounds.playSound("playerJump", 100.0f);
                velocity.y = -jumpSpeed;
                hasDoubleJumped = true;
            }
        }

        if (specialPressed == true) {
            if (crouching && gameMap.pickups["rocketPower"].collected) {
                rocketingUp = true;
                gameSounds.playSound("playerRocketing", 100.0f);
            }
            else if (!hasRocketed && gameMap.pickups["boostPower"].collected) {
                boostingAcross = true;
                hasRocketed = true;
                gameSounds.playSound("playerRocketing", 100.0f);
            }
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::X)) {
            if (!crouching) {
                if (timeSinceLastShot > shootingSpeed && timeSinceLastMissileShot > missileShootingSpeed && gameMap.pickups["shootPower"].collected) {
                    shooting = true;
                    bullets.push_back(PlayerBullet(playerPos, sf::Vector2f(16, 16), sf::Vector2f(8, 8), playerRect.facingLeft, gameSounds));
                    timeSinceLastShot = 0.0f;
                }
            }
            else {
                if (timeSinceLastMissileShot > missileShootingSpeed && gameMap.pickups["missilePower"].collected) {
                    shootingRocket = true;
                    missiles.push_back(Missile(playerPos, sf::Vector2f(32, 32), sf::Vector2f(16, 16), playerRect.facingLeft, gameSounds));
                    timeSinceLastMissileShot = 0.0f;
                }
            }
        }

        if (!crouching && !boostingAcross) {
            timeBoosted = 0.0f;

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
                playerRect.facingLeft = true;
                stationary = false;

                if (velocity.x > -walkSpeed) {
                    velocity.x -= ((walkSpeed * 10.0f) * FrameElapsed);
                }
                else {
                    velocity.x = -walkSpeed;
                }

            }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
                playerRect.facingLeft = false;
                stationary = false;

                if (velocity.x < walkSpeed) {
                    velocity.x += ((walkSpeed * 10.0f) * FrameElapsed);
                }
                else {
                    velocity.x = walkSpeed;
                }
            }
            else {
                velocity.x = 0.0f;
            }
        }

        if (boostingAcross) {
            hasDoubleJumped = false;
        }
    }

    void Step(float FrameElapsed, GameMap& gameMap, GameSounds& gameSounds) {
        HandleEvents(FrameElapsed, gameMap, gameSounds);

        timeSinceLastShot += FrameElapsed;
        timeSinceLastMissileShot += FrameElapsed;

        if (!(boostingAcross || rocketingUp)) {
            velocity.y += gravity.y * FrameElapsed;
        }
        else if (boostingAcross) {
            rocketingUp = false;
            timeRocketed = 0.0f;

            if (falling) {
                rocketingCollisionOffset = 0.05f;
            }
            if (playerRect.facingLeft) {
                velocity.x = -25.0f;
            }
            else {
                velocity.x = 25.0f;
            }

            timeBoosted += FrameElapsed;

            if (timeBoosted > rocketAcrossTime) {
                boostingAcross = false;
                timeBoosted = 0.0f;
            }
        }
        else if (rocketingUp) {
            velocity.y = -25.0f;

            timeRocketed += FrameElapsed;
            if (timeRocketed > rocketUpTime) {
                rocketingUp = false;
                timeRocketed = 0.0f;
            }
        }



        float newPlayerPosX = playerPos.x + velocity.x * FrameElapsed;
        float newPlayerPosY = playerPos.y + velocity.y * FrameElapsed;

        falling = true;
        float ratio = getRectRatio(playerRect);
        CollisionResult result = CollisionSolver(sf::Vector2f(playerPos.x, playerPos.y + rocketingCollisionOffset), velocity, playerWidthCollisionOffset * ratio, (1.0f - playerWidthCollisionOffset) * ratio, playerTopCollisionOffset * ratio, ratio, FrameElapsed, gameMap.collisionRects, boostingAcross);
        
        if (result.LeftCollision) {
            newPlayerPosX = ((int)newPlayerPosX + 1) - (playerWidthCollisionOffset * ratio); // works
            boostingAcross = false;
        }
        if (result.RightCollision) {
            newPlayerPosX = ((int)newPlayerPosX) + (playerWidthCollisionOffset * ratio); // works
            boostingAcross = false;
        }
        if (result.TopCollision) {
            newPlayerPosY = ((int)newPlayerPosY + 1) - (playerTopCollisionOffset * ratio); // works
            if (rocketingUp) {
                newPlayerPosY = newPlayerPosY + playerTopCollisionOffset;
            }
            velocity.y = 0;
        }
        if (result.BottomCollision) {    
            if (rocketingCollisionOffset != 0.0f) {
                rocketingCollisionOffset = 0;
            }
            else {
                newPlayerPosY = (int)newPlayerPosY;                   // works
            }
            velocity.y = 0;
            falling = false; // Player has a solid surface underfoot
        }



        if (boostingAcross) {
            velocity.y = 0.0f;
        }


        playerPos.x = newPlayerPosX;
        playerPos.y = newPlayerPosY;

        if (gameMap.hazardRects[playerPos.y + playerTopCollisionOffset][playerPos.x + playerWidthCollisionOffset] || gameMap.hazardRects[playerPos.y + (1.0f - playerTopCollisionOffset)][playerPos.x + playerWidthCollisionOffset]) {
            dead = true;
        }

        for (auto& enemy : gameMap.enemys) {
            if (PointCollisionSolver(playerPos, sf::Vector2f(enemy.enemyRect.pos.x + enemy.colOffsetX, enemy.enemyRect.pos.y + enemy.colOffsetY), (-1.0f + playerWidthCollisionOffset), (enemy.ratio - playerWidthCollisionOffset) - enemy.colOffsetX, -1.0f, enemy.ratio - playerTopCollisionOffset)) {
                dead = true;
            }
        }

        if (dead) {
            gameSounds.playSound("playerDie", 100.0f);
            playerPos = startPos;
            RunningTime += 20;
            dead = false;
            boostingAcross = false;
            rocketingUp = false;
            velocity = sf::Vector2f(0, 0);
            timeRocketed = 0.0f;
            timeBoosted = 0.0f;
        }
    }

    void Draw(GameMap& gameMap, sf::RenderTarget& target, sf::Vector2f playerCameraPos, sf::Vector2f cameraPos, float FrameElapsed, float TotalElapsed, GameSounds& gameSounds, MyText& text, float& popupTextScreenTime) {
        RunningTime += FrameElapsed;

        for (auto& bullet : bullets) {
            bullet.Step(gameMap, target, bulletTexture, cameraPos, FrameElapsed, TotalElapsed, gameSounds);
        }

        bullets.erase(std::remove_if(bullets.begin(), bullets.end(), [](const PlayerBullet& bullet) { return bullet.animation_stage >= 4; }),
            bullets.end());



        for (auto& missile : missiles) {
            missile.Step(gameMap, target, missileTexture, cameraPos, FrameElapsed, TotalElapsed, gameSounds);
        }

        missiles.erase(std::remove_if(missiles.begin(), missiles.end(), [](const Missile& missile) { return missile.animation_stage >= 4; }),
            missiles.end());



        // pick correct animations



        if (boostingAcross) { // rocketing sideways
            playerRect.startFrame = 5;
            playerRect.numberOfFrames = 1;
        }
        else if (rocketingUp) { // rocketing sideways
            playerRect.startFrame = 6;
            playerRect.numberOfFrames = 1;
        }
        else if (falling) { // falling
            playerRect.startFrame = 3;
            playerRect.numberOfFrames = 1;
        }
        else if (crouching) {
            playerRect.startFrame = 4;
            playerRect.numberOfFrames = 1;
        }
        else if (stationary) {
            playerRect.startFrame = 0;
            playerRect.numberOfFrames = 1;
        }
        else if (!stationary) {
            playerRect.startFrame = 1;
            playerRect.numberOfFrames = 2;
        }

        float ratio = getRectRatio(playerRect);

        for (auto& pickup : gameMap.pickups) {
            if (!pickup.second.collected) {
                if (PointCollisionSolver(playerPos, pickup.second.pickupRect.pos, -0.8f, 0.8f, -1.0f, 1.0f)) {
                    pickup.second.collected = true;
                    gameSounds.playSound("playerPowerup", 100.0f);
                    if (pickup.first == "redKey") {
                        popupTextScreenTime = 0.0f;
                        text.pos = sf::Vector2f(0.5f, 0.419f);
                        text.UpdateText("This is not a boxing glove.");
                        for (auto& door : gameMap.doors) {
                            if (door.type == 55) {
                                door.unlocked = true;
                            }
                        }
                    }
                    else if (pickup.first == "greenKey") {
                        popupTextScreenTime = 0.0f;
                        text.pos = sf::Vector2f(0.5f, 0.419f);
                        text.UpdateText("You can open Green Gates now!");
                        for (auto& door : gameMap.doors) {
                            if (door.type == 56) {
                                door.unlocked = true;
                            }
                        }
                    }
                    else if (pickup.first == "blueKey") {
                        popupTextScreenTime = 0.0f;
                        text.pos = sf::Vector2f(0.5f, 0.419f);
                        text.UpdateText("WHAT COULD THIS POSSIBLY DO!?!?!");
                        for (auto& door : gameMap.doors) {
                            if (door.type == 57) {
                                door.unlocked = true;
                            }
                        }
                    }
                    else if (pickup.first == "annihilation") {
                        popupTextScreenTime = 0.0f;
                        text.pos = sf::Vector2f(0.5f, 0.419f);
                        text.UpdateText("Hold X to ANNIHILATE.");
                        shootingSpeed = 0.1f;
                    }
                    else if (pickup.first == "jumpPower") {
                        popupTextScreenTime = 0.0f;
                        text.pos = sf::Vector2f(0.5f, 0.419f);
                        text.UpdateText("Press Z to jump!");
                    }
                    else if (pickup.first == "shootPower") {
                        popupTextScreenTime = 0.0f;
                        text.pos = sf::Vector2f(0.5f, 0.419f);
                        text.UpdateText("Press X to fire deadly lazorz!");
                    }
                    else if (pickup.first == "doubleJumpPower") {
                        popupTextScreenTime = 0.0f;
                        text.pos = sf::Vector2f(0.5f, 0.419f);
                        text.UpdateText("Now you can double-jump!");
                    }
                    else if (pickup.first == "boostPower") {
                        popupTextScreenTime = 0.0f;
                        text.pos = sf::Vector2f(0.5f, 0.419f);
                        text.UpdateText("Press C to rocket at ludicrous speed!");
                    }
                    else if (pickup.first == "rocketPower") {
                        popupTextScreenTime = 0.0f;
                        text.pos = sf::Vector2f(0.5f, 0.419f);
                        text.UpdateText("Hold down and press C to rocket skyward!");
                    }
                    else if (pickup.first == "missilePower") {
                        popupTextScreenTime = 0.0f;
                        text.pos = sf::Vector2f(0.5f, 0.419f);
                        text.UpdateText("Press X while ducking to fire missiles that smash red bricks.");
                    }
                }
            }
        }

        for (auto& checkpoint : gameMap.checkpoints) {
            if (!checkpoint.collected) {
                if (PointCollisionSolver(playerPos, checkpoint.checkpointRect.pos, -0.8f, 0.8f, -1.0f, 1.0f)) {
                    for (auto& checkpoint : gameMap.checkpoints) {
                        checkpoint.collected = false;
                    }
                    checkpoint.collected = true;
                    startPos = checkpoint.checkpointRect.pos;

                    popupTextScreenTime = 0.0f;
                    text.pos = sf::Vector2f(0.5f, 0.419f);
                    text.UpdateText("Robot Pattern Encoded!");
                }
            }
        }

        for (auto& door : gameMap.doors) {
            if (door.unlocked) {
                for (auto& doorTile : door.rects) {
                    if (PointCollisionSolver(playerPos, doorTile.destructableRect.pos, -0.90f, 0.90f, -1.0f, 1.0f)) {
                        if (timeBoosted != 0.0f) {
                            timeBoosted = 0.0f;
                            boostingAcross = false;
                        }

                        gameSounds.playSound("doorOpened", 100.0f);

                        door.opened = true;
                        break;
                    }
                }
            }
        }

        // draw
        if (rocketingUp) {
            rocketRect.draw(target, rocketTexture, playerCameraPos, TotalElapsed);
            rocketRect.facingLeft = !rocketRect.facingLeft;
        }
        playerRect.draw(target, playerTexture, playerCameraPos, TotalElapsed);
    }
};