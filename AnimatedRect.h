#pragma once
#include <SFML/Graphics.hpp>

class AnimatedRect
{
public:
    sf::Vector2f pos;
    sf::VertexArray vertices;

    sf::Vector2f sizeInWorld;
    sf::Vector2f sizeOnScreen;
    sf::Vector2f sizeOnImage;

    int startFrame;
    int numberOfFrames;
    float frameDuration;

    bool facingLeft;

    AnimatedRect() {}

    void Create(sf::Vector2f _pos, sf::Vector2f _sizeInWorld, sf::Vector2f _sizeOnScreen, sf::Vector2f _sizeOnImage, int _startFrame, int _numberOfFrames, float _frameDuration)
    {
        pos = _pos;
        vertices = sf::VertexArray(sf::Triangles, 6);

        sizeInWorld = _sizeInWorld;
        sizeOnScreen = _sizeOnScreen;
        sizeOnImage = _sizeOnImage;

        startFrame = _startFrame;
        numberOfFrames = _numberOfFrames;
        frameDuration = _frameDuration;

        facingLeft = false;
    }

    void draw(sf::RenderTarget& target, const sf::Texture& texture, sf::Vector2f posOffset, float elapsed, int mode = 0, std::vector<int> frameSequence = {}) {

        posOffset *= -1.0f;

        int spriteNumber = 0;
        if (mode == 0) {
            spriteNumber = (static_cast<int>(elapsed / frameDuration) % numberOfFrames) + startFrame;
        }
        else if (mode == 1) {
            int elapsedFrames = static_cast<int>(elapsed / frameDuration);
            int sequenceLength = frameSequence.size();
            int positionInSequence = elapsedFrames % sequenceLength;  // current position in the sequence
            spriteNumber = frameSequence[positionInSequence];  // Get the frame number from the sequence
        }

        int spritesPerRow = texture.getSize().x / sizeOnImage.x;
        int row = spriteNumber / spritesPerRow;
        int col = spriteNumber % spritesPerRow;

        sf::Vector2f drawpos;
        drawpos.x = (pos.x * sizeInWorld.x) + (posOffset.x * sizeInWorld.x);
        drawpos.y = (pos.y * sizeInWorld.y) + (posOffset.y * sizeInWorld.y);
        // define the 6 corners of the two triangles
        vertices[0].position = sf::Vector2f(drawpos.x, drawpos.y);
        vertices[1].position = sf::Vector2f(drawpos.x, drawpos.y + sizeOnScreen.y);
        vertices[2].position = sf::Vector2f(drawpos.x + sizeOnScreen.x, drawpos.y);
        vertices[3].position = vertices[2].position;
        vertices[4].position = vertices[1].position;
        vertices[5].position = sf::Vector2f(drawpos.x + sizeOnScreen.x, drawpos.y + sizeOnScreen.y);

        sf::Vector2f topLeft((col * sizeOnImage.x) + 0.01f, (row * sizeOnImage.y) + 0.01f);
        sf::Vector2f bottomRight((topLeft.x + sizeOnImage.x) - 0.01f, (topLeft.y + sizeOnImage.y) - 0.01f);
        // define the 6 matching texture coordinates
        if (!facingLeft) {
            vertices[0].texCoords = topLeft;
            vertices[1].texCoords = sf::Vector2f(topLeft.x, bottomRight.y);
            vertices[2].texCoords = sf::Vector2f(bottomRight.x, topLeft.y);
            vertices[3].texCoords = vertices[2].texCoords;
            vertices[4].texCoords = vertices[1].texCoords;
            vertices[5].texCoords = bottomRight;
        }
        else {
            vertices[0].texCoords = sf::Vector2f(bottomRight.x, topLeft.y);
            vertices[1].texCoords = bottomRight;
            vertices[2].texCoords = topLeft;
            vertices[3].texCoords = vertices[2].texCoords;
            vertices[4].texCoords = vertices[1].texCoords;
            vertices[5].texCoords = sf::Vector2f(topLeft.x, bottomRight.y);
        }

        sf::RenderStates states;
        // apply the tileset texture
        states.texture = &texture;

        // draw the vertex array
        target.draw(vertices, states);
    }
};