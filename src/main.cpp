#include <SFML/Graphics.hpp>
#include <vector>
#include <sstream>     // for string stream
#include <iomanip>     // for number formatting
#include "Player.hpp" // include player class


/**
 * TODO: * 1. Add collision detection function
 *    - Check if two rectangles intersect
 * 2. refactor the code to use this function
 *  Obstacle class refactor
 *  Obstacle Manager class refactor for spawning and managing obstacles
 *  Collision detection refactor
 *  Game class refactor
 * 
 * 3. Add a scoring system and display it on the screen and refactor the code
 * 
 * 4. Add a high score system and display it on the screen
 * 
 * 5. make the game more challenging by increasing the speed of obstacles over time
 *  randomize the obstacle size and position
 *  add bird obstacles that fly over the dino
 * 
 * 6. Add a game item system that gives the dino a power-up or a bonus life
 */

bool checkCollision(const sf::RectangleShape& rect1, const sf::RectangleShape& rect2) {
    sf::FloatRect bounds1 = rect1.getGlobalBounds();
    sf::FloatRect bounds2 = rect2.getGlobalBounds();
    return bounds1.intersects(bounds2);
}

int main(){
    // SFML2 방식: VideoMode(width, height)
    sf::RenderWindow window(
        sf::VideoMode(800, 600),     // ← 벡터 대신 두 개의 unsigned int
        "DinoRun",                   // 창 제목
        sf::Style::Default
    );

    // dinosaur settings
    Player dino(100, 400); // create a player object at position (100, 400)
    
    // time management for game score
    sf::Clock clock;
    std::vector<sf::RectangleShape> obstacles; // obstacles vector
    double obstacleTimer = 0; // obstacle timer

    // game status variable
    bool gameOver = false;
    double gameTime = 0.0;      // whole play time
    int currentScore = 0;       // current score (increasing with time)
    int highScore = 0;          // best score in the session

    // font settings
    sf::Font font;
    bool fontLoaded = font.loadFromFile("../fonts/arial.ttf"); // load font from file
    
    sf::Text gameOverText;      // game over text
    sf::Text scoreText;         // current score display
    sf::Text highScoreText;     // high score display
    if (fontLoaded) {
        // 1. game over text settings
        gameOverText.setFont(font);
        gameOverText.setString("GAME OVER! Press R to Restart");
        gameOverText.setCharacterSize(30);
        gameOverText.setFillColor(sf::Color::Black);
        gameOverText.setPosition(200, 250);

        // 2. score text settings
        scoreText.setFont(font);
        scoreText.setCharacterSize(24);
        scoreText.setFillColor(sf::Color::Black);
        scoreText.setPosition(20, 20);  // 화면 왼쪽 위

        // 3. high score text settings
        highScoreText.setFont(font);
        highScoreText.setCharacterSize(24);
        highScoreText.setFillColor(sf::Color::Black);
        highScoreText.setPosition(20, 50);  // 현재 점수 아래
    }

    // main game loop
    sf::Event ev;
    while (window.isOpen()){
        double deltaTime = clock.restart().asSeconds(); // time since last frame in seconds
        // SFML2 이벤트 처리: pollEvent(ev)
        while (window.pollEvent(ev)){   // ← 매개변수로 ev 전달
            if (ev.type == sf::Event::Closed)
                window.close();

            
            if (ev.type == sf::Event::KeyPressed) {
                // **JUMPING LOGIC** when space key is pressed
                if ((ev.key.code == sf::Keyboard::Space || ev.key.code == sf::Keyboard::Up)) {
                    dino.jump(); // call jump method from Player class
                }

                // **RESTART LOGIC** when R key is pressed
                // if game is over, pressing R will reset the game
                if (ev.key.code == sf::Keyboard::R && gameOver) {
                    // best score update
                    if (currentScore > highScore) {
                        highScore = currentScore;
                    }

                    // reseting game variables
                    gameOver = false;
                    obstacles.clear();
                    obstacleTimer = 0;
                    
                    dino.reset(); // reset player position and state

                    // score variables reset
                    gameTime = 0.0f;
                    currentScore = 0;
                }
            }
        }

        // JUMPING PHYSICS
        // if dino is jumping, apply gravity and update position
        if (!gameOver && dino.getIsJumping()) {
            dino.update(deltaTime); // update player physics
        }


        if (!gameOver) {
            // game time update
            gameTime += deltaTime;                          // time accumulation
            
            // hardness increases as time goes on
            double obstacleInterval = 2.0 - (gameTime * 0.01f);  // interval decreases as time goes on
            if (obstacleInterval < 1.0) obstacleInterval = 1.0f; // min interval limit
            
            double obstacleSpeed = 200 + (gameTime * 5);          // speed increases as time goes on
            if (obstacleSpeed > 400) obstacleSpeed = 400;        // max speed limit
            
            // Update score based on game time and speed
            currentScore = static_cast<int>(gameTime * 10 + (obstacleSpeed / 200 - 1)); // 10 points per second

            // OBSTACLE LOGIC
            // now obstacles are made faster as time goes on
            obstacleTimer += deltaTime;
            if (obstacleTimer > obstacleInterval) { // obstacle made **faster as time goes on**
                sf::RectangleShape newObstacle;
                newObstacle.setSize(sf::Vector2f(30, 50));  // 장애물 크기
                newObstacle.setFillColor(sf::Color::Red);   // 빨간색
                newObstacle.setOutlineColor(sf::Color::Black); // outline color
                newObstacle.setPosition(800, 410);          // 화면 오른쪽에서 시작
                obstacles.push_back(newObstacle);           // 벡터에 추가
                obstacleTimer = 0;  // 타이머 리셋
            }

            // obstacles moving and check collision and removing
            // move obstacles left, if they go off screen, remove them
            for (int i = obstacles.size() - 1; i >= 0; i--) {
                // 1. move left at 200 pixels per second
                obstacles[i].move(-obstacleSpeed * deltaTime, 0);  
    
                // 2. check collision with dino
                if (checkCollision(dino.getShape(), obstacles[i])) {
                    // if collision occurs, set game over
                    gameOver = true;
                    break; // game stops if collision occurs
                }
                
                // 3. if obstacle goes off screen, remove it
                if (obstacles[i].getPosition().x < -50) {
                    obstacles.erase(obstacles.begin() + i);
                }
            }
        }
           
        if (!gameOver) {
            // Update dino position
            dino.update(deltaTime); // update player physics
        }

        // score and high score text update
        // if font is loaded, update the text
        if (fontLoaded) {
            std::stringstream ss;
            ss << "Score: " << std::setfill('0') << std::setw(6) << currentScore;
            scoreText.setString(ss.str());
            
            std::stringstream hs;
            hs << "High: " << std::setfill('0') << std::setw(6) << highScore;
            highScoreText.setString(hs.str());
        }

        // Clear the window with a white background
        window.clear(sf::Color::White); 
        
        // Draw the dino and obstacles
        dino.render(window); // render player on screen            
        
        for (auto& obstacle : obstacles) {
            window.draw(obstacle); // 장애물 그리기
        }
        
        // game over logic
        if (gameOver && fontLoaded) {
            window.draw(gameOverText);
        }


        window.display();
    }
    return 0;
}
