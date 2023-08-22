#pragma once
#include "AnimatedRect.h"
#include <SFML/Audio.hpp>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <map>
#include <cmath>



struct CollisionResult {
    bool LeftCollision = false;
    bool RightCollision = false;
    bool TopCollision = false;
    bool BottomCollision = false;
};

CollisionResult CollisionSolver(sf::Vector2f Pos, sf::Vector2f velocity, float left, float right, float top, float bottom, float FrameElapsed, std::vector<std::vector<bool>>& collisionRects, bool forceCheckDown = false) {
    float newPosX = Pos.x + velocity.x * FrameElapsed;
    float newPosY = Pos.y + velocity.y * FrameElapsed;

    CollisionResult result;

    const float epsilon = 0.001f;

    // Moving Left
    if (velocity.x < 0) {
        if (collisionRects[static_cast<int>(Pos.y + top)][static_cast<int>(newPosX + left)] ||
            collisionRects[static_cast<int>(Pos.y + bottom * 0.99f)][static_cast<int>(newPosX + left)]) {
            result.LeftCollision = true;
            newPosX = static_cast<int>(newPosX) + right;
        }
    }
    // Moving Right
    else if (velocity.x > 0) {
        if (collisionRects[static_cast<int>(Pos.y + top)][static_cast<int>(newPosX + right)] ||
            collisionRects[static_cast<int>(Pos.y + bottom * 0.99f)][static_cast<int>(newPosX + right)]) {
            result.RightCollision = true;
            newPosX = static_cast<int>(newPosX) + left;
        }
    }

    // Moving Up
    if (velocity.y < 0) {
        if (collisionRects[static_cast<int>(newPosY + top)][static_cast<int>(Pos.x + left)] ||
            collisionRects[static_cast<int>(newPosY + top)][static_cast<int>(Pos.x + right * 0.99f)]) {
            result.TopCollision = true;
        }
    }
    // Moving Down
    else if (velocity.y > 0 || forceCheckDown) {
        if (collisionRects[static_cast<int>(newPosY + bottom)][static_cast<int>(Pos.x + left)] ||
            collisionRects[static_cast<int>(newPosY + bottom)][static_cast<int>(Pos.x + right * 0.99f)]) {
            result.BottomCollision = true;
        }
    }

    return result;
}

// Function to detect if Pos lies within the bounding box defined by Container and the offset parameters
bool PointCollisionSolver(sf::Vector2f Pos, sf::Vector2f Container, float left, float right, float top, float bottom) {
    bool withinBox = false;

    const float epsilon = 0.001f;

    if ((Pos.x) >= ((Container.x) + left - epsilon) && (Pos.x) <= ((Container.x) + right + epsilon)) {
        if ((Pos.y) >= ((Container.y) + top - epsilon) && (Pos.y) <= ((Container.y) + bottom + epsilon)) {
            withinBox = true;
        }
    }

    return withinBox;
}




std::string formatTimeFromSeconds(float seconds) {
    int minutes = static_cast<int>(seconds / 60);
    int remainingSeconds = static_cast<int>(seconds - minutes * 60);

    std::ostringstream formattedTime;

    formattedTime << std::setfill('0') << std::setw(1) << minutes << ":"
        << std::setfill('0') << std::setw(2) << remainingSeconds;

    return formattedTime.str();
}

float getRectRatio(AnimatedRect& rect) {
    float ratio = rect.sizeOnScreen.x / rect.sizeInWorld.x;
    return ratio;
}

bool isVisible(sf::RenderTarget& target, AnimatedRect& rect, sf::Vector2f posOffset) {
    float topLeftX = -1;
    float topLeftY = -1;

    posOffset *= -1.0f;

    float bottomRightX = (target.getView().getSize().x / rect.sizeInWorld.x);
    float bottomRightY = (target.getView().getSize().y / rect.sizeInWorld.y);
    float ratio = getRectRatio(rect);
    if ((rect.pos.x + ratio) + posOffset.x > topLeftX && (rect.pos.x - ratio) + posOffset.x < bottomRightX &&
        (rect.pos.y + ratio) + posOffset.y > topLeftY && (rect.pos.y - ratio) + posOffset.y < bottomRightY) {
        return true;
    }
    return false;
}

double myRound(double value, int multiplier) {
    return std::round(value * multiplier) / multiplier;
}

void getSprite(int tokenType, int& startFrame, int& numberOfFrames, float& frameDuration, bool& impassable, bool& hazardous) {
    startFrame = tokenType;

    if (tokenType == 16) { // deep acid
        numberOfFrames = 4;
        frameDuration = 0.1f;
        hazardous = true;
    }
    else if (tokenType == 20) { // surface acid
        numberOfFrames = 4;
        frameDuration = 0.1f;
        hazardous = true;
    }
    else if (tokenType >= 50 && tokenType <= 54) { // base tiles
        impassable = true;
    }

    else if (tokenType == 58) { // gold wall
        impassable = true;
    }

    else if (tokenType >= 63 && tokenType <= 65) { // other base tiles
        impassable = true;
    }
}
