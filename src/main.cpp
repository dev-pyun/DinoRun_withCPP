#include "Game.hpp"
#include <iostream>


/**
TODO: 
 * ✅ 1. Add collision detection function - COMPLETED
 *    - Check if two rectangles intersect
 * ✅ 2. refactor the code to use this function - IN PROGRESS
 *    ✅ Player class refactor - COMPLETED
 *    ✅ Obstacle class refactor - COMPLETED
 *    ✅ Obstacle Manager class refactor for spawning and managing obstacles - COMPLETED
 *    ✅ Collision detection refactor - CURRENT STEP
 *    ✅ Game class refactor - NEXT
 * 
 * ✅ 3. Add a scoring system and display it on the screen - COMPLETED
 * 
 * ✅ 4. Add a high score system and display it on the screen - COMPLETED
 * 
 * ⏳ 5. make the game more challenging by increasing the speed of obstacles over time - FUTURE
 *    ⏳ randomize the obstacle size and position - FUTURE
 *    ⏳ add bird obstacles that fly over the dino - FUTURE
 * 
 * ⏳ 6. Add a game item system that gives the dino a power-up or a bonus life - FUTURE
 * 
 * ⏳ 7. change the rectangle obstacle to a more complex shape like a bird or a cactus - FUTURE
 *    ⏳ add a background image and a foreground image - FUTURE
 *    ⏳ change dino from rectangular shape to dinosaur sprite - FUTURE
 *    ⏳ add a sound effect when the dino jumps or collides with an obstacle - FUTURE
 * 
 * ⏳ 8. Add a game over screen and a restart button - FUTURE
 * 
 * ⏳ 9. Add a main menu and a settings menu - FUTURE
 */


/**
 * The ultimate simplification: main.cpp as a pure entry point
 * 
 * This represents the culmination of our object-oriented refactoring journey.
 * From a 200+ line monolithic main function with scattered responsibilities,
 * we now have a clean, simple entry point that delegates all work to the
 * appropriate specialized classes.
 * 
 * Design Achievement:
 * - Single Responsibility: main() only creates and runs the game
 * - Separation of Concerns: All game logic is properly encapsulated
 * - Error Handling: Centralized in the Game class
 * - Resource Management: Handled through RAII and smart pointers
 * - Extensibility: New features can be added without touching main()
 * 
 * This transformation demonstrates the power of object-oriented design:
 * complexity is not eliminated, but it is organized, contained, and
 * made manageable through proper abstraction and encapsulation.
 */

 int main() {
    std::cout << "=== DinoRun: Complete Object-Oriented Architecture ===" << std::endl;
    std::cout << "Initializing game systems..." << std::endl;
    
    try {
        // Create the game instance - this initializes all systems
        Game game;
        
        std::cout << "Starting game..." << std::endl;
        
        // Run the complete game - everything is handled internally
        return game.run();
        
    } catch (const std::exception& e) {
        // Centralized error handling
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return -1;
    } catch (...) {
        // Catch any unexpected errors
        std::cerr << "Unknown fatal error occurred" << std::endl;
        return -2;
    }
}


// // Original main function for reference

// int main(){
//     // SFML2 방식: VideoMode(width, height)
//     sf::RenderWindow window(
//         sf::VideoMode(800, 600),     // ← 벡터 대신 두 개의 unsigned int
//         "DinoRun",                   // 창 제목
//         sf::Style::Default
//     );

//     // dinosaur settings
//     Player dino(100, 400); // create a player object at position (100, 400)
//     // Obstacle management system application 
//     ObstacleManager obstacleManager;     // Obstacle management system application
    
//     // time management for game score
//     sf::Clock clock;

//     // game status variable
//     bool gameOver = false;
//     double gameTime = 0.0;      // whole play time
//     int currentScore = 0;       // current score (increasing with time)
//     int highScore = 0;          // best score in the session

//     // font settings
//     sf::Font font;
//     bool fontLoaded = font.loadFromFile("../fonts/arial.ttf"); // load font from file
    
//     sf::Text gameOverText;      // game over text
//     sf::Text scoreText;         // current score display
//     sf::Text highScoreText;     // high score display

//     if (fontLoaded) {
//         // 1. game over text settings
//         gameOverText.setFont(font);
//         gameOverText.setString("GAME OVER! Press R to Restart");
//         gameOverText.setCharacterSize(30);
//         gameOverText.setFillColor(sf::Color::Black);
//         gameOverText.setPosition(200, 250);

//         // 2. score text settings
//         scoreText.setFont(font);
//         scoreText.setCharacterSize(24);
//         scoreText.setFillColor(sf::Color::Black);
//         scoreText.setPosition(20, 20);  // Top-Left corner

//         // 3. high score text settings
//         highScoreText.setFont(font);
//         highScoreText.setCharacterSize(24);
//         highScoreText.setFillColor(sf::Color::Black);
//         highScoreText.setPosition(20, 50);  // Below current score
//     }

//     // main game loop
//     sf::Event ev;
//     while (window.isOpen()){
//         double deltaTime = clock.restart().asSeconds(); // time since last frame in seconds
//         // SFML2 이벤트 처리: pollEvent(ev)
//         while (window.pollEvent(ev)){   // ← 매개변수로 ev 전달
//             if (ev.type == sf::Event::Closed)
//                 window.close();

            
//             if (ev.type == sf::Event::KeyPressed) {
//                 // **JUMPING LOGIC** when space key is pressed
//                 if ((ev.key.code == sf::Keyboard::Space || ev.key.code == sf::Keyboard::Up)) {
//                     dino.jump(); // call jump method from Player class
//                 }

//                 // **RESTART LOGIC** when R key is pressed
//                 // if game is over, pressing R will reset the game
//                 if (ev.key.code == sf::Keyboard::R && gameOver) {
//                     // best score update
//                     if (currentScore > highScore) {
//                         highScore = currentScore;
//                     }

//                     // reseting game variables
//                     gameOver = false;
//                     gameTime = 0.0;
//                     currentScore = 0;
                    
//                     dino.reset(); // reset player position and state
//                     obstacleManager.clear(); // clear obstacle manager state
//                 }
//             }
//         }

//         // JUMPING PHYSICS
//         // if dino is jumping, apply gravity and update position
//         if (!gameOver && dino.getIsJumping()) {
//             dino.update(deltaTime); // update player physics
//         }


//         // ==== GAME LOGIC UPDATE : collision manager update ====
//         if (!gameOver) {
//             // Time tracking for score and difficulty calculation
//             gameTime += deltaTime;
            
//             // Score calculation with speed bonus (maintaining original formula)
//             // Base score: 10 points per second + speed bonus for increased difficulty
//             double currentSpeed = obstacleManager.getCurrentSpeed();
//             currentScore = static_cast<int>(gameTime * 10 + (currentSpeed / 200 - 1));
            
//             // Player physics update - handles jump mechanics internally
//             dino.update(deltaTime);
            
//             // Obstacle system update - handles spawning, movement, difficulty scaling
//             obstacleManager.update(deltaTime, gameTime);
            
//             // ===== Collision Detection - Now Using CollisionManager! =====
//             // Before: scattered collision logic mixed with game logic
//             // After: clean, centralized collision detection through dedicated manager
//             if (CollisionManager::checkPlayerObstacleCollision(dino, obstacleManager)) {
//                 gameOver = true;
//             }
//         }

//         // Score and high score text
//         // if font is loaded, update the text
//         if (fontLoaded) {
//             std::stringstream ss;
//             ss << "Score: " << std::setfill('0') << std::setw(6) << currentScore;
//             scoreText.setString(ss.str());
            
//             std::stringstream hs;
//             hs << "High: " << std::setfill('0') << std::setw(6) << highScore;
//             highScoreText.setString(hs.str());
//         }

//         // Clear the window with a white background
//         window.clear(sf::Color::White); 
        
//         // Draw the dino and obstacles
//         dino.render(window); // render player on screen            
//         obstacleManager.render(window); // render all obstacles on the screen
        
//         // UI text rendering
//         if (fontLoaded) {
//             window.draw(scoreText);      // Display current score
//             window.draw(highScoreText);  // Display high score
            
//             if (gameOver) {
//                 window.draw(gameOverText);  // Display game over message
//             }
//         }

//         window.display();
//     }
//     return 0;
// }
