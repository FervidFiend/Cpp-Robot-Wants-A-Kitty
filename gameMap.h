#pragma once
#include "HelperFunctions.h"
#include "Kitty.h"
#include "Enemy.h"
#include "SoundHelper.h"
#include "Bullet.h"



class ParticleSystem {
public:
    struct Particle {
        AnimatedRect particle;
        sf::Vector2f velocity;
        float lifetime;
        float age;

        Particle(sf::Vector2f pos, sf::Vector2f vel, float lifetime) : velocity(vel), lifetime(lifetime), age(0) {
            int type = rand() % 4;
            particle.Create(pos, sf::Vector2f(32, 32), sf::Vector2f(16, 16), sf::Vector2f(8, 8), type, 1, 120.0f);
        }

        bool isAlive() const {
            return age < lifetime;
        }

        void update(float FrameElapsed) {
            particle.pos += (velocity * FrameElapsed);
            age += FrameElapsed;
        }
    };

    std::vector<Particle> particles;

    ParticleSystem() {

    }

    void SpawnParticle(sf::Vector2f pos, sf::Vector2f velocity, float lifetime) {
        particles.emplace_back(pos, velocity, lifetime);
    }

    void Step(sf::RenderTarget& target, sf::Texture& texture, sf::Vector2f cameraPos, float FrameElapsed, float TotalElapsed, GameSounds& gameSounds) {
        for (auto it = particles.begin(); it != particles.end();) {
            if (it->isAlive()) {
                if (isVisible(target, it->particle, cameraPos)) {
                    it->particle.draw(target, texture, cameraPos, TotalElapsed);
                }
                it->update(FrameElapsed);
                ++it;
            }
            else {
                it = particles.erase(it);
            }
        }
    }
};


class Pickup {
public:
    AnimatedRect pickupRect;

    bool collected = false;

    Pickup() {

    }

    Pickup(sf::Vector2f pos, int type) {
        pickupRect.Create(pos, sf::Vector2f(32, 32), sf::Vector2f(32, 32), sf::Vector2f(16, 16), type, 1, 120.0f);
    }

    void Step(sf::RenderTarget& target, sf::Texture& texture, sf::Vector2f cameraPos, float TotalElapsed) {
        if (!collected) {
            if (isVisible(target, pickupRect, cameraPos)) {
                pickupRect.draw(target, texture, cameraPos, TotalElapsed);
            }
        }
    }
};

class CheckPoint {
public:
    AnimatedRect checkpointRect;

    bool collected = false;

    CheckPoint(sf::Vector2f pos, int type) {
        checkpointRect.Create(pos, sf::Vector2f(32, 32), sf::Vector2f(32, 32), sf::Vector2f(16, 16), type, 1, 120.0f);
    }

    void Step(sf::RenderTarget& target, sf::Texture& texture, sf::Vector2f cameraPos, float TotalElapsed) {
        if (collected) {
            checkpointRect.startFrame = 31;
        }
        else {
            checkpointRect.startFrame = 30;
        }
        if (isVisible(target, checkpointRect, cameraPos)) {
            checkpointRect.draw(target, texture, cameraPos, TotalElapsed);
        }
    }
};

class Destructable {
public:
    AnimatedRect destructableRect;

    bool destroyed = false;

    Destructable(sf::Vector2f pos, int type) {
        destructableRect.Create(pos, sf::Vector2f(32, 32), sf::Vector2f(32, 32), sf::Vector2f(16, 16), type, 1, 120.0f);
    }

    void Step(sf::RenderTarget& target, sf::Texture& texture, sf::Vector2f cameraPos, float TotalElapsed) {
        if (isVisible(target, destructableRect, cameraPos)) {
            destructableRect.draw(target, texture, cameraPos, TotalElapsed);
        }
    }
};

class Door {
public:
    std::vector<Destructable> rects;
    int type;

    bool unlocked = false;
    bool opened = false;

    Door(sf::Vector2f pos, int _type) {
        type = _type;
        rects.push_back(Destructable(pos, type));
    }

    void AddToDoor(sf::Vector2f pos) {
        rects.push_back(Destructable(pos, type));
    }

    void Step(sf::RenderTarget& target, sf::Texture& texture, sf::Vector2f cameraPos, float TotalElapsed, std::vector<std::vector<bool>>& collisionRects) {
        for (auto& doorTile : rects) {
            if (isVisible(target, doorTile.destructableRect, cameraPos)) {
                doorTile.destructableRect.draw(target, texture, cameraPos, TotalElapsed);
            }
            if (opened) {
                doorTile.destroyed = true;
                collisionRects[doorTile.destructableRect.pos.y][doorTile.destructableRect.pos.x] = false;
            }

            doorTile.Step(target, texture, cameraPos, TotalElapsed);
        }
    }
};


class GameMap {
public:
    ParticleSystem particleSystem;

    sf::Texture bulletTexture;

    sf::RenderTexture backgroundRenderTexture;
    sf::Sprite backgroundSprite;
    sf::RenderTexture foregroundRenderTexture;
    sf::Sprite foregroundSprite;

    sf::Texture particleTexture;

    sf::Texture EnemyAlienTexture;
    sf::Texture EnemyAlienTexture2;
    sf::Texture EnemyAlienTexture3;

    sf::Texture BossAlienTexture;

    sf::Texture gameMapTexture;

    std::vector<std::vector<AnimatedRect>> gameMapRects;
    std::vector<std::vector<AnimatedRect>> backgroundRects;
    std::vector<std::vector<bool>> collisionRects;
    std::vector<std::vector<bool>> hazardRects;

    float paralax = 0.5f;

    sf::Vector2f spriteScreenSize = sf::Vector2f(32, 32); // size of each tile on the screen
    sf::Vector2f spriteImageSize = sf::Vector2f(16, 16); // size of each tile in the sprite sheet

    sf::Vector2i gameMapSize;
    sf::Vector2i backgroundSize;

    sf::Vector2f cameraPos = sf::Vector2f(0, 0);






    Kitty kitty;

    std::map<std::string, Pickup> pickups;

    std::vector<CheckPoint> checkpoints;

    std::vector<Door> doors;

    std::vector<Destructable> destructableTiles;

    std::vector<Enemy> enemys;



    std::vector<EnemyBullet> enemyBullets;



    GameMap() {
        particleTexture.loadFromFile("images/AlienBlastImage.png");
        particleTexture.setSmooth(false);

        bulletTexture.loadFromFile("images/LaserImage.png");
        bulletTexture.setSmooth(false);

        gameMapTexture.loadFromFile("images/TilesImage.png");
        gameMapTexture.setSmooth(false);

        EnemyAlienTexture.loadFromFile("images/AlienImage.png");
        EnemyAlienTexture2.loadFromFile("images/AlienImage2.png");
        EnemyAlienTexture3.loadFromFile("images/AlienImage3.png");
        EnemyAlienTexture.setSmooth(false);
        EnemyAlienTexture2.setSmooth(false);
        EnemyAlienTexture3.setSmooth(false);

        BossAlienTexture.loadFromFile("images/BossImage.png");
        BossAlienTexture.setSmooth(false);
    }

    void initializeTextures(sf::Vector2f screenSize) {
        backgroundRenderTexture.create(screenSize.x, screenSize.y);
        backgroundSprite.setTexture(backgroundRenderTexture.getTexture());
        foregroundRenderTexture.create(screenSize.x, screenSize.y);
        foregroundSprite.setTexture(foregroundRenderTexture.getTexture());
    }

    void CreategameMap(std::string filePath) {

        gameMapRects.clear();
        backgroundRects.clear();
        collisionRects.clear();
        hazardRects.clear();

        kitty = Kitty();

        pickups.clear();

        checkpoints.clear();

        doors.clear();

        destructableTiles.clear();

        enemys.clear();

        std::ifstream file(filePath.c_str());

        std::string line;
        int y = 0;
        int x = 0;
        while (std::getline(file, line)) {
            std::vector<AnimatedRect> gameMapRow;
            std::vector<bool> collisionRow;
            std::vector<bool> hazardRow;
            x = 0;
            std::stringstream ss(line);
            std::string token;

            while (std::getline(ss, token, ',')) {
                token.erase(std::remove_if(token.begin(), token.end(), ::isspace), token.end());
                AnimatedRect tile;
                sf::Vector2f tilePos = sf::Vector2f(x, y);

                int startFrame = 0;
                int numberOfFrames = 1;
                float frameDuration = 120.0f;
                bool impassable = false;
                bool hazardous = false;
                bool isPickup = false;
                bool isEnemy = false;

                int tileType = std::stoi(token);



                if (tileType == 3) {
                    Enemy enemy(sf::Vector2f(x, y), &EnemyAlienTexture, sf::Vector2f(2, 0), 3, "redWalker");
                    enemy.colOffsetX = 0.1f;
                    enemy.enemyRect.frameDuration = 0.07f;
                    enemys.push_back(enemy);
                    tileType = 0;
                }
                else if (tileType == 4) {
                    Pickup jumpPower(sf::Vector2f(x, y), tileType);
                    pickups["jumpPower"] = jumpPower;
                    tileType = 0;
                }
                else if (tileType == 5) {
                    Pickup shootPower(sf::Vector2f(x, y), tileType);
                    pickups["shootPower"] = shootPower;
                    tileType = 0;
                }
                else if (tileType == 7) {
                    Pickup doubleJumpPower(sf::Vector2f(x, y), tileType);
                    pickups["doubleJumpPower"] = doubleJumpPower;
                    tileType = 0;
                }
                else if (tileType == 8) {
                    Pickup rocketPower(sf::Vector2f(x, y), tileType);
                    pickups["rocketPower"] = rocketPower;
                    tileType = 0;
                }
                else if (tileType == 9) {
                    Pickup boostPower(sf::Vector2f(x, y), tileType);
                    pickups["boostPower"] = boostPower;
                    tileType = 0;
                }
                else if (tileType == 10) {
                    Enemy enemy(sf::Vector2f(x, y), &EnemyAlienTexture2, sf::Vector2f(0, 2), 1, "blueFloater");
                    enemy.enemyRect.frameDuration = 0.05f;
                    enemys.push_back(enemy);
                    tileType = 0;
                }
                else if (tileType == 11) {
                    Pickup redKey(sf::Vector2f(x, y), tileType);
                    pickups["redKey"] = redKey;
                    tileType = 0;
                }
                else if (tileType == 12) {
                    Pickup greenKey(sf::Vector2f(x, y), tileType);
                    pickups["greenKey"] = greenKey;
                    tileType = 0;
                }
                else if (tileType == 13) {
                    Pickup blueKey(sf::Vector2f(x, y), tileType);
                    pickups["blueKey"] = blueKey;
                    tileType = 0;
                }
                else if (tileType == 14) {
                    Enemy boss(sf::Vector2f(x, y), &BossAlienTexture, sf::Vector2f(1, 0), 30, "boss", sf::Vector2f(128, 128), sf::Vector2f(64, 64), 1, {0, 1, 2, 3, 2, 1});
                    boss.colOffsetX = 0.2f;
                    boss.colOffsetY = 1.8f;
                    enemys.push_back(boss);
                    tileType = 0;
                }
                else if (tileType == 15) {
                    Pickup Annihilation(sf::Vector2f(x, y), tileType);
                    pickups["annihilation"] = Annihilation;
                    tileType = 0;
                }
                else if (tileType == 24) {
                    Enemy enemy(sf::Vector2f(x, y), &EnemyAlienTexture3, sf::Vector2f(0, 0), 4, "drip");
                    enemy.mode = 1;
                    enemy.enemyRect.frameDuration = (1.0f / 8.0f);
                    enemy.frameSequence = { 1, 0, 1, 2, 1, 0, 1, 3 };
                    enemys.push_back(enemy);
                    tileType = 0;
                }
                else if (tileType == 25) {
                    Pickup missilePower(sf::Vector2f(x, y), tileType);
                    pickups["missilePower"] = missilePower;
                    tileType = 0;
                }
                else if (tileType == 30) {
                    CheckPoint checkpoint(sf::Vector2f(x, y), tileType);
                    checkpoints.push_back(checkpoint);
                    tileType = 0;
                }
                else if (tileType >= 55 && tileType <= 57) {
                    bool added = false;
                    for (auto& door : doors) {
                        if (door.type == tileType) {
                            for (auto& doorTile : door.rects) {
                                if (PointCollisionSolver(sf::Vector2f(x, y), doorTile.destructableRect.pos, -0.0f, 0.0f, -1.0f, 1.0f)) {
                                    door.AddToDoor(sf::Vector2f(x, y));
                                    added = true;
                                    break;
                                }
                            }
                        }
                    }

                    if (!added) {
                        Door door(sf::Vector2f(x, y), tileType);
                        doors.push_back(door);
                    }

                    tileType = 0;
                    impassable = true;
                }
                else if (tileType == 66) {
                    Destructable destructableTile(sf::Vector2f(x, y), tileType);
                    destructableTiles.push_back(destructableTile);
                    tileType = 0;
                    impassable = true;
                }

                getSprite(tileType, startFrame, numberOfFrames, frameDuration, impassable, hazardous);

                tile.Create(tilePos, sf::Vector2f(32, 32), spriteScreenSize, spriteImageSize, startFrame, numberOfFrames, frameDuration);
                gameMapRow.push_back(tile);
                collisionRow.push_back(impassable);
                hazardRow.push_back(hazardous);
                x += 1;
            }
            gameMapRects.push_back(gameMapRow);
            collisionRects.push_back(collisionRow);
            hazardRects.push_back(hazardRow);
            y += 1;
        }
        gameMapSize = sf::Vector2i(x, y);

        CreateBackground();
    }

    void CreateBackground() {
        int y = 0;
        int x = 0;
        for (y = 0; y < gameMapSize.y; y++) {
            std::vector<AnimatedRect> backgroundRow;
            for (x = 0; x < gameMapSize.x; x++) {
                AnimatedRect tile;
                sf::Vector2f tilePos = sf::Vector2f(x, y);

                int tileTypex = x % 2;
                int tileTypey = (y % 2) * 2;  // 0 for even rows, 2 for odd rows
                int tileType = 26 + tileTypex + tileTypey;

                tile.Create(tilePos, sf::Vector2f(32, 32), spriteScreenSize, spriteImageSize, tileType, 1, 120.0f);
                backgroundRow.push_back(tile);
            }
            backgroundRects.push_back(backgroundRow);
        }
        backgroundSize = sf::Vector2i(x, y);
    }

    void Step(bool& playerDead, sf::Vector2f playerPos, sf::RenderTarget& target, GameSounds& gameSounds, sf::Vector2f cameraPos, float FrameElapsed, float TotalElapsed, float& Player_RunningTime) {
        backgroundRenderTexture.clear(sf::Color::Transparent);
        foregroundRenderTexture.clear(sf::Color::Transparent);

        sf::Vector2f backgroundCameraPos = sf::Vector2f(cameraPos.x * paralax, cameraPos.y * paralax);

        // draw background
        for (int x = 0; x < backgroundSize.x; x++) {
            for (int y = 0; y < backgroundSize.y; y++) {
                if (isVisible(target, backgroundRects[y][x], backgroundCameraPos)) {
                    backgroundRects[y][x].draw(backgroundRenderTexture, gameMapTexture, backgroundCameraPos, TotalElapsed);
                }
            }
        }
        backgroundRenderTexture.display();

        // draw foreground
        for (int x = 0; x < gameMapSize.x; x++) {
            for (int y = 0; y < gameMapSize.y; y++) {
                if (isVisible(target, gameMapRects[y][x], cameraPos)) {
                    gameMapRects[y][x].draw(foregroundRenderTexture, gameMapTexture, cameraPos, TotalElapsed);
                }
            }
        }
        foregroundRenderTexture.display();

        target.draw(backgroundSprite);
        target.draw(foregroundSprite);

        kitty.Step(target, cameraPos, TotalElapsed);

        for (auto& pickup : pickups) {
            pickup.second.Step(target, gameMapTexture, cameraPos, TotalElapsed);
        }

        for (auto& checkpoint : checkpoints) {
            checkpoint.Step(target, gameMapTexture, cameraPos, TotalElapsed);
        }

        for (auto& door : doors) {
            door.Step(target, gameMapTexture, cameraPos, TotalElapsed, collisionRects);
        }

        doors.erase(std::remove_if(doors.begin(), doors.end(), [](const Door& door) {
            return door.opened;
            }), doors.end());

        for (auto& bullet : enemyBullets) {
            bullet.Step(playerDead, playerPos, collisionRects, target, bulletTexture, cameraPos, FrameElapsed, TotalElapsed, gameSounds);
        }

        enemyBullets.erase(std::remove_if(enemyBullets.begin(), enemyBullets.end(), [](const EnemyBullet& bullet) { return bullet.animation_stage >= 4; }),
            enemyBullets.end());

        for (auto& tile : destructableTiles) {
            tile.Step(target, gameMapTexture, cameraPos, TotalElapsed);
        }

        destructableTiles.erase(std::remove_if(destructableTiles.begin(), destructableTiles.end(), [](const Destructable& tile) {
            return tile.destroyed;
            }), destructableTiles.end());

        for (auto& enemy : enemys) {
            enemy.Step(target, cameraPos, FrameElapsed, TotalElapsed, gameSounds, collisionRects, enemyBullets);
            if (enemy.health <= 0) {
                for (int _i = 0; _i < 20; _i++) {
                    float partvelx = ((rand() % 1000) - 500) / 100;
                    float partvely = ((rand() % 1000) - 500) / 100;
                    particleSystem.SpawnParticle(enemy.enemyRect.pos + sf::Vector2f(0.25, 0.25), sf::Vector2f(partvelx, partvely), 1.0f);
                }
                Player_RunningTime -= 1.0f;
            }
        }

        particleSystem.Step(target, particleTexture, cameraPos, FrameElapsed, TotalElapsed, gameSounds);

        enemys.erase(std::remove_if(enemys.begin(), enemys.end(), [](const Enemy& enemy) {
            return enemy.health <= 0;
            }), enemys.end());
    }
};