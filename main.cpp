#include "Player.h"
#include "gameMap.h"
#include "SoundHelper.h"

#include <windows.h>

float exponentialFade(float popupTextScreenTime, float popupTextDesiredScreenTime, float popupTextDesiredFadeOutTime, float factor) {
    float elapsedTime = std::max(popupTextScreenTime - popupTextDesiredScreenTime, 0.0f);
    float fadeFactor = pow(elapsedTime / popupTextDesiredFadeOutTime, factor);

    return fadeFactor;
}







std::vector<std::vector<AnimatedRect>> backgroundRects; // Container for your background tiles
sf::Vector2i gameMapSize(22, 15); // Size of your map in tiles
sf::Vector2f spriteScreenSize = sf::Vector2f(32, 32); // size of each tile on the screen
sf::Vector2f spriteImageSize = sf::Vector2f(16, 16); // size of each tile in the sprite sheet

sf::Texture backgroundTexture; // Texture that will be used for the background tiles
sf::Texture playerTexture; // Texture that will be used for the background tiles
sf::Texture kittyTexture; // Texture that will be used for the background tiles

AnimatedRect menuPlayerRect;
AnimatedRect menuKittyRect;

float anim_time = 0.0f;

void CreateBackground() {
    backgroundTexture.loadFromFile("images/TilesImage.png");
    backgroundTexture.setSmooth(false);
    playerTexture.loadFromFile("images/PlayerImage.png");
    playerTexture.setSmooth(false);
    kittyTexture.loadFromFile("images/menu/KittyImage.png");
    kittyTexture.setSmooth(false);

    for (int y = 0; y < gameMapSize.y; ++y) {
        std::vector<AnimatedRect> backgroundRow;
        for (int x = 0; x < gameMapSize.x; ++x) {
            AnimatedRect tile;
            sf::Vector2f tilePos(x * spriteScreenSize.x, y * spriteScreenSize.y); // Position in pixels

            int tileType;

            if (y == gameMapSize.y - 1) {
                int tileTypex = x % 2;
                tileType = 51 + tileTypex;
            }
            else {
                int tileTypex = x % 2;
                int tileTypey = (y % 2) * 2;  // 0 for even rows, 2 for odd rows
                tileType = 26 + tileTypex + tileTypey; // Whatever formula you use to set the tile type
            }

            tile.Create(sf::Vector2f(x, y), sf::Vector2f(32, 32), spriteScreenSize, spriteImageSize, tileType, 1, 120.0f);
            backgroundRow.push_back(tile);
        }
        backgroundRects.push_back(backgroundRow);
    }

    menuPlayerRect.Create(sf::Vector2f(2, 13), sf::Vector2f(32, 32), spriteScreenSize, spriteImageSize, 1, 2, 0.08f);
    menuKittyRect.Create(sf::Vector2f(18.75, 11), sf::Vector2f(32, 32), sf::Vector2f(64, 64), sf::Vector2f(32, 32), 0, 1, 120.0f);
}

float tileSpeed = 2.0f; // Speed of background movement in pixels per frame
float globalOffsetX = 0.0f; // Global X offset for all tiles

void UpdateBackground(float deltaTime) {
    // Update the global offset
    globalOffsetX -= tileSpeed * deltaTime;

    // If it goes off the screen, adjust it back
    while (globalOffsetX <= -2) {
        globalOffsetX += 2;
    }
}

sf::Vector2f kitty_offset;// = sf::Vector2f(1, 1);

void DrawBackground(sf::RenderWindow& window, float deltaTime) {
    UpdateBackground(deltaTime); // First update the positions

    anim_time += deltaTime;

    kitty_offset.x = 1.25f * std::sin(3.3f * anim_time);
    kitty_offset.y = 0.5f * std::cos(4.3f * anim_time);

    for (auto& row : backgroundRects) {
        for (auto& tile : row) {
            tile.draw(window, backgroundTexture, sf::Vector2f(-globalOffsetX, 0), 0);
        }
    }
    menuPlayerRect.draw(window, playerTexture, sf::Vector2f(0, 0), anim_time);
    menuKittyRect.draw(window, kittyTexture, kitty_offset, anim_time);
}





class MenuButton {
public:
    sf::Texture buttonOnTexture;
    sf::Texture buttonOffTexture;
    sf::Sprite buttonSprite;
    sf::Vector2f buttonPos;

    MyText buttonText;

    bool mouseOver = false;
    bool beingPressed = false;

    bool clicked = false;

    MenuButton() {}

    MenuButton(std::string textContents, sf::Font& font, int fontSize, sf::Vector2f _screenSize, sf::Vector2f _pos, std::string buttonOnFile, std::string buttonOffFile) {
        buttonPos = _pos;

        buttonOnTexture.loadFromFile(buttonOnFile);
        buttonOffTexture.loadFromFile(buttonOffFile);
        buttonSprite.setScale(2, 2);
        buttonSprite.setPosition(_pos);

        buttonText = MyText(textContents, font, fontSize, _screenSize, _pos, false);
    }

    void Step(sf::Vector2i mousePos, bool mousePressed) {
        clicked = false;

        sf::Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));

        if (buttonSprite.getGlobalBounds().contains(mousePosF)) {
            mouseOver = true;
        } else {
            mouseOver = false;
        }

        if (mousePressed || !mouseOver) {
            buttonSprite.setTexture(buttonOffTexture);
        }
        else if (mouseOver) {
            buttonSprite.setTexture(buttonOnTexture);
        }

        if (mousePressed && mouseOver) {
            beingPressed = true;
        }

        if (beingPressed && !mousePressed && mouseOver) {
            clicked = true;
            beingPressed = false;
        }
        else if (!mouseOver) {
            beingPressed = false;
        }
    }

    void Draw(sf::RenderTarget& target, sf::RenderTarget& TextTexture) {
        // Draw the button sprite
        target.draw(buttonSprite);

        // Update the position of the text to be centered with respect to the sprite
        sf::FloatRect spriteBounds = buttonSprite.getGlobalBounds();
        sf::FloatRect textBounds = buttonText.text.getLocalBounds();

        sf::Vector2f spriteCenter(
            spriteBounds.left + spriteBounds.width / 2,
            spriteBounds.top + spriteBounds.height / 2
        );

        sf::Vector2f textCenter(
            textBounds.left + textBounds.width / 2,
            textBounds.top + textBounds.height / 2
        );

        // Set the position of the text so that its center aligns with the center of the sprite
        buttonText.text.setPosition(
            spriteCenter.x - textCenter.x,
            spriteCenter.y - textCenter.y
        );

        // Draw the button text
        TextTexture.draw(buttonText.text);
    }
};


class RobotWantsAKitty {
public:
    bool onTitleScreen = true;

    sf::RenderWindow window;

    sf::Vector2i lastWindowPosition;

    bool windowHasFocus = true;

    bool mouseClicked = false;

    sf::Font font;
    MyText timeText;
    MyText resumeText;
    MyText popupText;

    MyText copyrightCreditsText;
    MyText musicCreditsText;

    MyText startText;

    MenuButton playButton;
    MenuButton huhButton;
    MenuButton maxGamesButton;
    MenuButton backButton;

    sf::Texture cursorTexture;
    sf::Sprite cursorSprite;

    sf::Texture titleScreenTexture;
    sf::Sprite titleScreen;

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

    sf::Music song0;
    sf::Music song1;
    sf::Music song2;
    sf::Music song3;

    GameSounds gameSounds;

    float aspectRatio;
    sf::View view;

    RobotWantsAKitty(unsigned int screenWidth, unsigned int screenHeight) {
        song0.openFromFile("sounds/menu/titleSong.mp3");
        song1.openFromFile("sounds/song1.mp3");
        song2.openFromFile("sounds/song2.mp3");
        song3.openFromFile("sounds/song3.mp3");

        song0.setVolume(100.0f);
        song1.setVolume(100.0f);
        song2.setVolume(100.0f);
        song3.setVolume(100.0f);

        cursorTexture.loadFromFile("images/menu/CursorImage.png");
        cursorSprite.setTexture(cursorTexture);
        cursorSprite.setScale(2, 2);

        titleScreenTexture.loadFromFile("images/menu/TitleImage.png");
        titleScreenTexture.setSmooth(false);
        titleScreen.setTexture(titleScreenTexture);
        titleScreen.setScale(2, 2);
        titleScreen.setOrigin(-titleScreen.getLocalBounds().width * 0.5f, -titleScreen.getLocalBounds().height * 0.0f);
        titleScreen.setPosition(0, 20);

        pauseScreenTexture.loadFromFile("images/Instructions.png");
        pauseScreenTexture.setSmooth(false);
        pauseScreen.setTexture(pauseScreenTexture);
        pauseScreen.setPosition(5, 5);

        winScreenTexture.loadFromFile("images/WinPicture.png");
        winScreenTexture.setSmooth(false);
        winScreen.setTexture(winScreenTexture);
        winScreen.setScale(float(screenWidth) / float(winScreenTexture.getSize().x), float(screenHeight) / float(winScreenTexture.getSize().y));

        previousPlayerPosition = player.playerRect.pos;

        if (!font.loadFromFile("fonts/Nokia Cellphone.ttf")) {
            std::cout << "font not loaded" << std::endl;
        }

        sf::Vector2f screenSize = sf::Vector2f(screenWidth, screenHeight);

        playButton = MenuButton("Play Game", font, 16, screenSize, sf::Vector2f(screenWidth * 0.15f, screenHeight * 0.575f), "images/menu/BtnOnImage.png", "images/menu/BtnOffImage.png");
        huhButton = MenuButton("Huh!?", font, 16, screenSize, sf::Vector2f(screenWidth * 0.65f, screenHeight * 0.575f), "images/menu/BtnOnImage.png", "images/menu/BtnOffImage.png");
        maxGamesButton = MenuButton("Play More Games at Maxgames.com", font, 16, screenSize, sf::Vector2f(screenWidth * 0.205f, screenHeight * 0.75f), "images/menu/BtnOnBigImage.png", "images/menu/BtnOffBigImage.png");

        backButton = MenuButton("Back", font, 16, screenSize, sf::Vector2f(screenWidth * 0.4f, screenHeight * 0.885f), "images/menu/BtnOnImage.png", "images/menu/BtnOffImage.png");

        timeText = MyText("", font, 16, screenSize, sf::Vector2f(4, 5));

        resumeText = MyText("[Click or P to resume]", font, 16, screenSize, sf::Vector2f(0.5f, 1.0f - (10.0f / screenHeight)), true);

        popupText = MyText("Arrow keys to move!", font, 16, screenSize, sf::Vector2f(0.5f, 0.425f), true);

        startText = MyText("Collect power-ups to jump, shoot, and more!", font, 16, screenSize, sf::Vector2f(0.5f, 0.614f), true);

        copyrightCreditsText = MyText("Copyright 2010, Hamumu Software", font, 16, screenSize, sf::Vector2f(screenWidth * 0.005f, screenHeight * 0.96f));
        musicCreditsText = MyText("Music by DrPetter", font, 16, screenSize, sf::Vector2f(screenWidth * 0.72f, screenHeight * 0.96f));

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

        CreateBackground();

        resetGame();
    }

    void resetGame() {
        gameMap.CreategameMap("maps/testMap.csv");

        victory = false;

        player.reset();

        //song1.play();
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
                else if (event.type == sf::Event::Resized)
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
                    song0.pause();
                    break;
                }
                else if (event.type == sf::Event::GainedFocus) {
                    windowHasFocus = true;
                    song0.play();
                    break;
                }
                else if (event.type == sf::Event::MouseButtonPressed) {
                    if (event.mouseButton.button == sf::Mouse::Left) {
                        mouseClicked = true;
                    }
                }
                else if (event.type == sf::Event::MouseButtonReleased) {
                    if (event.mouseButton.button == sf::Mouse::Left) {
                        mouseClicked = false;
                    }
                }
                else if (event.type == sf::Event::KeyPressed) {
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

            sf::Vector2i mousePos = sf::Mouse::getPosition(window);

            FrameClock.restart();

            window.clear(sf::Color::Black);
            TextTexture.clear(sf::Color::Transparent);
            PopupTextTexture.clear(sf::Color::Transparent);

            if (!victory && !onTitleScreen) {
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

            if (paused || !windowHasFocus) {
                window.draw(pauseScreen);

                if (onTitleScreen) {
                    backButton.Step(mousePos, mouseClicked);
                    backButton.Draw(window, TextTexture);

                    cursorSprite.setPosition(mousePos.x, mousePos.y);
                    window.draw(cursorSprite);

                    if (backButton.clicked) {
                        paused = false;
                    }
                }
                else {
                    TextTexture.draw(resumeText.text);
                }

                TextTexture.display();

                TextSprite.setTexture(TextTexture.getTexture());

                window.draw(TextSprite);
                window.display();
            }
            else if (onTitleScreen) {
                if (song0.getStatus() == sf::Music::Stopped) {
                    song0.play();
                }

                DrawBackground(window, FrameTime);

                playButton.Step(mousePos, mouseClicked);
                huhButton.Step(mousePos, mouseClicked);
                maxGamesButton.Step(mousePos, mouseClicked);

                playButton.Draw(window, TextTexture);
                huhButton.Draw(window, TextTexture);
                maxGamesButton.Draw(window, TextTexture);


                TextTexture.draw(copyrightCreditsText.text);
                TextTexture.draw(musicCreditsText.text);


                TextTexture.display();
                TextSprite.setTexture(TextTexture.getTexture());
                window.draw(TextSprite);


                window.draw(titleScreen);




                cursorSprite.setPosition(mousePos.x, mousePos.y);
                window.draw(cursorSprite);
                window.display();

                if (playButton.clicked) {
                    onTitleScreen = false;
                    song0.stop();
                }
                else if (huhButton.clicked) {
                    paused = true;
                }
                else if (maxGamesButton.clicked) {
                    ShellExecute(NULL, L"open", L"https://maxgames.com", NULL, NULL, SW_SHOWNORMAL);
                }
            }
            else if (victory) {
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