#pragma once

class Kitty {
public:
    sf::Texture kittyTexture;
    AnimatedRect kittyRect;

    Kitty() {
        kittyTexture.loadFromFile("images/KittyImage.png");
        kittyTexture.setSmooth(false);
        kittyRect.Create(sf::Vector2f(10, 64), sf::Vector2f(32, 32), sf::Vector2f(32, 32), sf::Vector2f(16, 16), 0, 3, 0.25f);
    }

    void Step(sf::RenderTarget& target, sf::Vector2f cameraPos, float TotalElapsed) {
        std::vector<int> frameSequence = { 2, 0, 1, 0 };
        if (isVisible(target, kittyRect, cameraPos)) {
            kittyRect.draw(target, kittyTexture, cameraPos, TotalElapsed, 1, frameSequence);
        }
    }
};