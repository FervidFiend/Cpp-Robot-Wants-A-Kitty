#include "Player.h"
#include "gameMap.h"
#include "SoundHelper.h"

float exponentialFade(float popupTextScreenTime, float popupTextDesiredScreenTime, float popupTextDesiredFadeOutTime, float factor) {
    float elapsedTime = std::max(popupTextScreenTime - popupTextDesiredScreenTime, 0.0f);
    float fadeFactor = pow(elapsedTime / popupTextDesiredFadeOutTime, factor);

    return fadeFactor;
}


class RobotWantsAKitty {
public:
    sf::RenderWindow window;

    sf::Vector2i lastWindowPosition;

    bool windowHasFocus = true;

    sf::Font font;
    MyText timeText;
    MyText resumeText;
    MyText popupText;

    MyText startText;

    sf::Texture pauseScreenTexture;
    sf::Sprite pauseScreen;

    sf::Texture winScreenTexture;
    sf::Sprite winScreen;

    sf::Vector2f desiredCameraPos = sf::Vector2f(0, 0);
    sf::Vector2f currCameraPos = sf::Vector2f(0, 0);

    bool paused = false;
    bool victory = false;

    sf::Clock FrameClock;
    float FrameTime;
    float AnimationTime;

    sf::Vector2f previousPlayerPosition;


    sf::RenderTexture TextTexture;
    sf::RenderTexture PopupTextTexture;
    sf::Sprite TextSprite;
    sf::Sprite PopupTextSprite;


    float popupTextScreenTime = 0.0f;
    float popupTextDesiredScreenTime = 4.0f;
    float popupTextDesiredFadeOutTime = 1.0f;


    Player player;
    GameMap gameMap;

    sf::Music song1;
    sf::Music song2;
    sf::Music song3;

    GameSounds gameSounds;

    float aspectRatio;
    sf::View view;

    RobotWantsAKitty(unsigned int screenWidth, unsigned int screenHeight) {
        song1.openFromFile("sounds/song1.mp3");
        song2.openFromFile("sounds/song2.mp3");
        song3.openFromFile("sounds/song3.mp3");

        song1.setVolume(100.0f);
        song2.setVolume(100.0f);
        song3.setVolume(100.0f);

        pauseScreenTexture.loadFromFile("images/Instructions.png");
        pauseScreenTexture.setSmooth(false);
        pauseScreen.setTexture(pauseScreenTexture);
        pauseScreen.setPosition(5, 5);

        winScreenTexture.loadFromFile("images/WinPicture.png");
        winScreenTexture.setSmooth(false);
        winScreen.setTexture(winScreenTexture);
        winScreen.setScale(float(screenWidth) / float(winScreenTexture.getSize().x), float(screenHeight) / float(winScreenTexture.getSize().y));

        previousPlayerPosition = player.playerRect.pos;

        font.loadFromFile("fonts/Nokia Cellphone.ttf");

        sf::Vector2f screenSize = sf::Vector2f(screenWidth, screenHeight);

        timeText = MyText("", font, 16, screenSize, sf::Vector2f(4, 5));

        resumeText = MyText("[Click or P to resume]", font, 16, screenSize, sf::Vector2f(0.5f, 1.0f - (10.0f / screenHeight)), true);

        popupText = MyText("Arrow keys to move!", font, 16, screenSize, sf::Vector2f(0.5f, 0.425f), true);

        startText = MyText("Collect power-ups to jump, shoot, and more!", font, 16, screenSize, sf::Vector2f(0.5f, 0.614f), true);

        TextTexture.create(screenWidth, screenHeight);
        TextTexture.setSmooth(false);
        PopupTextTexture.create(screenWidth, screenHeight);
        PopupTextTexture.setSmooth(false);

        window.create(sf::VideoMode(screenWidth, screenHeight), "My Robot Wants A Kitty C++");
        window.setMouseCursorVisible(false);

        window.setKeyRepeatEnabled(false);

        lastWindowPosition = window.getPosition();

        player.Create(sf::Vector2f(screenWidth, screenHeight));

        currCameraPos = player.playerPos - player.cameraPos;

        gameMap.initializeTextures(sf::Vector2f(screenWidth, screenHeight));

        view = window.getDefaultView();
        view.setSize(window.getSize().x, window.getSize().y);
        aspectRatio = view.getSize().x / view.getSize().y; 

        FrameClock.restart();
        FrameTime = FrameClock.getElapsedTime().asSeconds();
        AnimationTime = 0.0f;

        resetGame();
    }

    void resetGame() {
        gameMap.CreategameMap("maps/testMap.csv");

        victory = false;

        player.reset();

        song1.play();
    }

    void Run() {
        while (window.isOpen()) {

            player.jumpPressed = false;
            player.specialPressed = false;

            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    window.close();
                    break;
                }
                if (event.type == sf::Event::Resized)
                {
                    sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);

                    // Get the aspect ratio of the new window size
                    float newAspectRatio = static_cast<float>(event.size.width) / static_cast<float>(event.size.height);

                    // Compare the aspect ratio of the new size to the original aspect ratio
                    if (newAspectRatio > aspectRatio)
                    {
                        visibleArea.width = aspectRatio * visibleArea.height;
                        visibleArea.left = (event.size.width - visibleArea.width) / 2.0f;
                    }
                    else
                    {
                        visibleArea.height = visibleArea.width / aspectRatio;
                        visibleArea.top = (event.size.height - visibleArea.height) / 2.0f;
                    }

                    view.setViewport(sf::FloatRect(
                        static_cast<float>(visibleArea.left) / event.size.width,
                        static_cast<float>(visibleArea.top) / event.size.height,
                        static_cast<float>(visibleArea.width) / event.size.width,
                        static_cast<float>(visibleArea.height) / event.size.height));
                    window.setView(view);

                    break;
                }
                else if (event.type == sf::Event::LostFocus) {
                    windowHasFocus = false;
                    break;
                }
                if (event.type == sf::Event::GainedFocus) {
                    windowHasFocus = true;
                    break;
                }
                if (event.type == sf::Event::KeyPressed) {
                    if (event.key.code == sf::Keyboard::C) {
                        player.specialPressed = true;
                    }

                    if (event.key.code == sf::Keyboard::P) {
                        paused = !paused;
                    }

                    if (event.key.code == sf::Keyboard::Up || event.key.code == sf::Keyboard::Z) {
                        player.jumpPressed = true;
                    }
                    break;
                }
            }

            FrameClock.restart();

            window.clear(sf::Color::Black);
            TextTexture.clear(sf::Color::Transparent);
            PopupTextTexture.clear(sf::Color::Transparent);

            if (!victory) {
                if (song1.getStatus() == sf::Music::Stopped && song2.getStatus() == sf::Music::Stopped && song3.getStatus() == sf::Music::Stopped) {
                    int nextSong = rand() % 3;
                    if (nextSong == 0) {
                        song1.play();
                    }
                    else if (nextSong == 1) {
                        song2.play();
                    }
                    else if (nextSong == 2) {
                        song3.play();
                    }
                }
            }

            if (victory) {
                window.draw(winScreen);

                timeText.UpdateText("Total Time: " + formatTimeFromSeconds(player.RunningTime));
                TextTexture.draw(timeText.text);
                TextTexture.display();
                TextSprite.setTexture(TextTexture.getTexture());
                window.draw(TextSprite);

                if (player.jumpPressed) {
                    resetGame();
                }

                window.display();
            }
            else if (windowHasFocus && !paused) {

                desiredCameraPos = player.playerPos - player.cameraPos;

                sf::Vector2f playerDiff = currCameraPos - desiredCameraPos;


                // bind camera to gameMap
                desiredCameraPos.x = std::max(0.0f, desiredCameraPos.x); // left side of screen
                desiredCameraPos.x = std::min(desiredCameraPos.x, gameMap.gameMapSize.x - player.screenSizeInWorldSize.x); // right side of screen

                desiredCameraPos.y = std::max(0.0f, desiredCameraPos.y); // top of screen
                desiredCameraPos.y = std::min(desiredCameraPos.y, gameMap.gameMapSize.y - player.screenSizeInWorldSize.y); // bottom of screen


                sf::Vector2f diff = currCameraPos - desiredCameraPos;
                sf::Vector2f moveAmnt = diff * (FrameTime * 2.5f);
                currCameraPos = currCameraPos - moveAmnt;

                gameMap.Step(player.dead, player.playerPos, window, gameSounds, sf::Vector2f(myRound(currCameraPos.x, 16), myRound(currCameraPos.y, 16)), FrameTime, AnimationTime, player.RunningTime);

                player.Step(FrameTime, gameMap, gameSounds);
                player.Draw(gameMap, window, playerDiff, sf::Vector2f(myRound(currCameraPos.x, 16), myRound(currCameraPos.y, 16)), FrameTime, AnimationTime, gameSounds, popupText, popupTextScreenTime);

                timeText.UpdateText(formatTimeFromSeconds(player.RunningTime));
                TextTexture.draw(timeText.text);
                TextTexture.display();
                TextSprite.setTexture(TextTexture.getTexture());
                window.draw(TextSprite);

                popupTextScreenTime += FrameTime;
                if (popupTextScreenTime <= popupTextDesiredScreenTime) {
                    PopupTextTexture.draw(startText.text);
                    PopupTextTexture.draw(popupText.text);
                    PopupTextTexture.display();
                    PopupTextSprite.setTexture(PopupTextTexture.getTexture());

                    sf::Color PopupTextSpriteColor = PopupTextSprite.getColor();

                    PopupTextSpriteColor.a = 255;

                    PopupTextSprite.setColor(PopupTextSpriteColor);
                    window.draw(PopupTextSprite);
                } 
                else if (popupTextScreenTime <= popupTextDesiredScreenTime + popupTextDesiredFadeOutTime) {
                    PopupTextTexture.draw(startText.text);
                    PopupTextTexture.draw(popupText.text);
                    PopupTextTexture.display();
                    PopupTextSprite.setTexture(PopupTextTexture.getTexture());

                    sf::Color PopupTextSpriteColor = PopupTextSprite.getColor();

                    float exponentialFactor = exponentialFade(popupTextScreenTime, popupTextDesiredScreenTime, popupTextDesiredFadeOutTime, 4);

                    int alphaValue = static_cast<int>(255 * exponentialFactor);

                    alphaValue = std::min(alphaValue, 255);
                    alphaValue = std::max(0, alphaValue);

                    PopupTextSpriteColor.a = 255 - alphaValue;

                    PopupTextSprite.setColor(PopupTextSpriteColor);
                    window.draw(PopupTextSprite);
                }

                if (PointCollisionSolver(player.playerPos, gameMap.kitty.kittyRect.pos, -0.5f, 0.5f, -0.5f, 0.5f)) {
                    victory = true;
                    gameSounds.playSound("victory", 50.0f);
                    song1.stop();
                    song2.stop();
                    song3.stop();

                }

                window.display();

                AnimationTime += FrameTime;

                if (startText.text.getString() != "" && AnimationTime > popupTextDesiredScreenTime + popupTextDesiredFadeOutTime) {
                    startText.UpdateText("");
                }
            }
            else {
                window.draw(pauseScreen);
                TextTexture.draw(resumeText.text);
                TextTexture.display();

                TextSprite.setTexture(TextTexture.getTexture());

                window.draw(TextSprite);
                window.display();
            }

            FrameTime = FrameClock.restart().asSeconds();
            window.setTitle("My Robot Wants A Kitty C++, FPS: " + std::to_string(1.0f / FrameTime));
            FrameTime = std::min((1.0f / 15.0f), FrameTime); // will switch to fixed time (aka slowdown) below certain fps to prevent wall clipping and other collision breakdowns
        }
    }
};


int main() {
    const unsigned int screenWidth = 640;
    const unsigned int screenHeight = screenWidth * 0.75; // old style aspect ratio
  
    RobotWantsAKitty robotWantsAKitty(screenWidth, screenHeight);

    robotWantsAKitty.Run();

    return 0;
}