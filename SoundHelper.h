#pragma once
#include <SFML/Audio.hpp>


#include <map>

class GameSounds {
public:
    std::map<std::string, sf::SoundBuffer> soundBuffers;
    std::map<std::string, sf::Sound> sounds;

    GameSounds() {
        initializeSounds();
    }

    void initializeSounds() {
        addSound("shoot", "sounds/ShootSound.mp3");
        addSound("shotHit", "sounds/ShotHitSound.mp3");
        addSound("playerDie", "sounds/PlayerDieSound.mp3");
        addSound("playerJump", "sounds/PlayerJumpSound.mp3");
        addSound("doorOpened", "sounds/PlayerDoorSound.mp3");
        addSound("missileShoot", "sounds/MissileShootSound.mp3");
        addSound("missileHit", "sounds/MissileHitSound.mp3");
        addSound("playerRocketing", "sounds/PlayerRocketingSound.mp3");
        addSound("playerPowerup", "sounds/PlayerPowerupSound.mp3");
        addSound("victory", "sounds/VictorySound.mp3");
        addSound("alienDie", "sounds/AlienDieSound.mp3");
        addSound("alienHit", "sounds/AlienHitSound.mp3");
        addSound("alienBossHit", "sounds/AlienBossHitSound.mp3");
        // Add other sound effects here...
    }

    void addSound(const std::string& name, const std::string& filePath) {
        sf::SoundBuffer buffer;
        buffer.loadFromFile(filePath);
        soundBuffers[name] = buffer;

        sf::Sound sound;
        sound.setBuffer(soundBuffers[name]);
        sounds[name] = sound;
    }

    void playSound(const std::string& name, float volume) {
        if (sounds.count(name) > 0) {
            sounds[name].setVolume(volume);
            sounds[name].play();
        }
    }
};


