#include <SFML/Graphics.hpp>
#include <vector>

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
    sf::RectangleShape dino;
    dino.setSize(sf::Vector2f(40, 60));          // 크기 설정 (가로40, 세로60)
    dino.setFillColor(sf::Color::Green);         // green color
    dino.setOutlineColor(sf::Color::Black);      // outline color
    dino.setOutlineThickness(2);                 // outline thickness
    
    double dinoX = 100; // dino initial x coordinate
    double dinoY = 400; // dino initial y coordinate
    double velocityY = 0; // dino y-axis velocity
    bool isJumping = false; // dino jumping status
    
    // time management for game score
    sf::Clock clock;
    std::vector<sf::RectangleShape> obstacles; // obstacles vector
    double obstacleTimer = 0; // obstacle timer

    // game status variable
    bool gameOver = false;

    // font settings
    sf::Font font;
    bool fontLoaded = font.loadFromFile("../fonts/arial.ttf"); // load font from file
    
    sf::Text gameOverText;
    if (fontLoaded) {
        gameOverText.setFont(font);
        gameOverText.setString("GAME OVER! Press R to Restart");
        gameOverText.setCharacterSize(30);
        gameOverText.setFillColor(sf::Color::Black);
        gameOverText.setPosition(200, 250);
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
                if (ev.key.code == sf::Keyboard::Space || ev.key.code == sf::Keyboard::Up && !isJumping && !gameOver) {
                    isJumping = true;
                    velocityY = -400;  // power to jump
                }

                // **RESTART LOGIC** when R key is pressed
                // if game is over, pressing R will reset the game
                if (ev.key.code == sf::Keyboard::R && gameOver) {
                    // reseting game variables
                    gameOver = false;
                    obstacles.clear();
                    obstacleTimer = 0;
                    dinoY = 400;
                    velocityY = 0;
                    isJumping = false;
                }
            }
        }

        // JUMPING PHYSICS
        // if dino is jumping, apply gravity and update position
        if (!gameOver && isJumping) {
            velocityY += 800 * deltaTime;
            dinoY += velocityY * deltaTime;
            
            if (dinoY >= 400) {
                dinoY = 400;
                velocityY = 0;
                isJumping = false;
            }
        }


        if (!gameOver) {
            // OBSTACLE LOGIC
            // every 2 seconds, create a new obstacle
            
            obstacleTimer += deltaTime;
            if (obstacleTimer > 2.0) { // obstacle made every 2 seconds
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
                obstacles[i].move(-200 * deltaTime, 0);  
    
                // 2. check collision with dino
                if (checkCollision(dino, obstacles[i])) {
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
            dino.setPosition(dinoX, dinoY);  // 새로 추가
        }

        // Clear the window with a white background
        window.clear(sf::Color::White); 
        
        // Draw the dino and obstacles
        window.draw(dino);              

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
