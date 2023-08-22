#pragma once
#include <SFML/Graphics.hpp>

class MyText {
public:
    sf::Text text;

    sf::Vector2f screenSize;

    sf::Vector2f pos;
    sf::Vector2f drawPos;
    bool centered;

    MyText() {

    }

    MyText(std::string textContents, sf::Font& font, int fontSize, sf::Vector2f _screenSize, sf::Vector2f _pos, bool _centered = false) {
        text = sf::Text(textContents, font, fontSize);
        screenSize = _screenSize;
        pos = _pos;
        centered = _centered;

        UpdateText(textContents);
    }

    void UpdateText(std::string newString) {
        drawPos = pos;
        text.setString(newString);

        if (centered) {
            // Center the text on the given position
            sf::FloatRect textBounds = text.getLocalBounds();
            drawPos = sf::Vector2f(pos.x * screenSize.x, pos.y * screenSize.y);
            drawPos.x = int(drawPos.x - textBounds.width / 2.0f);
            drawPos.y = int(drawPos.y - textBounds.height / 2.0f);
        }

        text.setPosition(drawPos);
    }

};