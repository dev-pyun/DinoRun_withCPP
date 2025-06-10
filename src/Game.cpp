#include "Game.hpp"
#include "Player.hpp"
#include "ObstacleManager.hpp"
#include "CollisionManager.hpp"
#include "TextureManager.hpp"   // NEW: Add TextureManager include
#include <iostream>

// Static constant definitions - centralized game configuration
const int Game::WINDOW_WIDTH = 800;
const int Game::WINDOW_HEIGHT = 600;
const int Game::TARGET_FPS = 60;
const std::string Game::WINDOW_TITLE = "DinoRun - Complete OOP Architecture";

// ===== Core Lifecycle Methods =====

Game::Game() 
    : currentState(GameState::PLAYING),  // Start directly in playing state for now
      previousState(GameState::PLAYING),
      gameTime(0.0),
      currentScore(0),
      highScore(0),
      isRunning(false),
      fontLoaded(false) {
    
    // Initialize all subsystems in proper order
    initializeWindow();
    initializeTextureSystem();  // NEW: Initialize texture system first
    loadResources();
    initializeSystems();
    initializeUI();
    
    std::cout << "Game system initialized successfully!" << std::endl;
}

Game::~Game() {
    std::cout << "Game system shutting down..." << std::endl;
    
    // Clean up texture manager
    TextureManager::getInstance().cleanup();
    
    // Smart pointers handle other cleanup automatically
    std::cout << "All systems cleaned up successfully." << std::endl;
}

int Game::run() {
    std::cout << "Starting main game loop with sprite system..." << std::endl;
    isRunning = true;
    
    // Verify texture system is ready
    TextureManager& textureManager = TextureManager::getInstance();
    if (textureManager.getLoadedTextureCount() == 0) {
        std::cerr << "Warning: No textures loaded. Game may not display correctly." << std::endl;
    }
    
    // Main game loop
    while (isRunning && window.isOpen()) {
        double deltaTime = frameClock.restart().asSeconds();
        
        // The three pillars of game development: Handle, Update, Render
        handleEvents();
        update(deltaTime);
        render();
        
        // Maintain consistent performance
        maintainFrameRate();
        
        // Optional: Log debug info every few seconds
        static double debugTimer = 0.0;
        debugTimer += deltaTime;
        if (debugTimer > 10.0 && currentState == GameState::PLAYING) {  // Every 5 seconds
            logDebugInfo();
            debugTimer = 0.0;
        }
    }
    
    std::cout << "Game loop ended. Final score: " << currentScore << std::endl;
    return 0;
}

// ===== Initialization Methods =====

void Game::initializeWindow() {
    // Create window with modern SFML settings
    window.create(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), 
                  WINDOW_TITLE, 
                  sf::Style::Titlebar | sf::Style::Close);
    
    // Enable VSync for smooth rendering
    window.setVerticalSyncEnabled(true);
    
    // Set frame rate limit as backup to VSync
    window.setFramerateLimit(TARGET_FPS);
    
    std::cout << "Window initialized: " << WINDOW_WIDTH << "x" << WINDOW_HEIGHT << std::endl;
}

// ===== NEW: Texture system initialization method =====
void Game::initializeTextureSystem() {
    std::cout << "Initializing texture management system..." << std::endl;
    
    // Get TextureManager instance and initialize it
    TextureManager& textureManager = TextureManager::getInstance();
    
    if (!textureManager.initialize()) {
        std::cerr << "Warning: TextureManager initialization failed. Using fallback graphics." << std::endl;
    } else {
        std::cout << "TextureManager initialized successfully!" << std::endl;
    }
    
    // Print debug information about loaded textures
    textureManager.printDebugInfo();
}

void Game::initializeSystems() {
    // Create game systems using smart pointers for automatic memory management
    // The make_unique function is the modern C++ way to create smart pointers
    player = std::make_unique<Player>(100, 400);
    obstacleManager = std::make_unique<ObstacleManager>();
    
    std::cout << "Game systems initialized: Player, ObstacleManager" << std::endl;
}

void Game::loadResources() {
    // Try multiple font paths for better compatibility across different systems
    std::vector<std::string> fontPaths = {
        "C:/Windows/Fonts/arial.ttf",           // Windows standard path
        "/System/Library/Fonts/Arial.ttf",      // macOS path
        "/usr/share/fonts/truetype/arial.ttf",  // Linux path
        "assets/fonts/arial.ttf",               // Local assets folder
        "arial.ttf"                             // Current directory fallback
        "../fonts/arial.ttf"                    // Relative path for development
    };
    
    fontLoaded = false;
    for (const auto& path : fontPaths) {
        if (loadFont(path)) {
            fontLoaded = true;
            std::cout << "Font loaded successfully from: " << path << std::endl;
            break;
        }
    }
    
    if (!fontLoaded) {
        std::cerr << "Warning: Could not load any font. Text will not display." << std::endl;
    }
}

void Game::initializeUI() {
    if (!fontLoaded) {
        std::cerr << "Cannot initialize UI: Font not loaded" << std::endl;
        return;
    }
    
    // Configure all UI text elements with consistent styling
    configureText(gameOverText, "GAME OVER! Press R to Restart", 30, 
                  sf::Vector2f(200, 250), sf::Color::Red);
    
    configureText(scoreText, "Score: 000000", 24, 
                  sf::Vector2f(20, 20), sf::Color::Black);  // Top-Left corner
    
    configureText(highScoreText, "High: 000000", 24, 
                  sf::Vector2f(20, 50), sf::Color::Black);  // Below current score
    
    configureText(instructionText, "Press SPACE BAR or UP key to Jump", 18, 
                  sf::Vector2f(20, WINDOW_HEIGHT - 30), sf::Color::Green); // Bottom-Left corner
    
    std::cout << "UI elements initialized successfully" << std::endl;
}

// ===== Core Game Loop Methods =====

void Game::handleEvents() {
    while (window.pollEvent(currentEvent)) {
        // Handle universal events (window close, etc.)
        if (currentEvent.type == sf::Event::Closed) {
            isRunning = false;
            return;
        }
        
        // Handle state-specific events
        switch (currentState) {
            case GameState::PLAYING:
                handlePlayingStateEvents();
                break;
            case GameState::GAME_OVER:
                handleGameOverStateEvents();
                break;
            default:
                // Other states can be handled here in the future
                break;
        }
    }
}

void Game::update(double deltaTime) {
    // Update based on current game state
    switch (currentState) {
        case GameState::PLAYING:
            updatePlayingState(deltaTime);
            break;
        case GameState::GAME_OVER:
            updateGameOverState(deltaTime);
            break;
        default:
            // Other states can be updated here in the future
            break;
    }
    
    // Always update UI regardless of state
    updateScoreDisplays();
}

void Game::render() {
    window.clear(sf::Color::White);
    
    // Render game world elements
    renderGameWorld();
    
    // Render UI overlay
    renderUI();
    
    window.display();
}

// ===== State Management Methods =====

void Game::changeState(GameState newState) {
    if (currentState == newState) return;  // No change needed
    
    previousState = currentState;
    currentState = newState;
    
    std::cout << "State changed to: " << static_cast<int>(newState) << std::endl;
    
    // Handle state-specific initialization
    switch (newState) {
        case GameState::PLAYING:
            // Reset game for new play session
            resetGame();
            break;
        case GameState::GAME_OVER:
            // Finalize score and update high score
            updateHighScore();
            break;
        default:
            break;
    }
}

void Game::handlePlayingStateEvents() {
    if (currentEvent.type == sf::Event::KeyPressed) {
        // Handle jump input
        if (currentEvent.key.code == sf::Keyboard::Space || 
            currentEvent.key.code == sf::Keyboard::Up) {
            player->jump();
        }
        
        // Debug: Allow manual state change for testing
        if (currentEvent.key.code == sf::Keyboard::Escape) {
            changeState(GameState::GAME_OVER);
        }

        if (currentEvent.key.code == sf::Keyboard::Down) {
            player->startDucking();
        }
    }

    if(currentEvent.type == sf::Event::KeyReleased) {
        if (currentEvent.key.code == sf::Keyboard::Down) {
            player->stopDucking();
        }
    }
}

void Game::handleGameOverStateEvents() {
    if (currentEvent.type == sf::Event::KeyPressed) {
        // Handle restart input
        if (currentEvent.key.code == sf::Keyboard::R) {
            changeState(GameState::PLAYING);
        }
        
        // Future: Handle menu navigation, settings, etc.
    }
}

void Game::updatePlayingState(double deltaTime) {
    // Update game time for score calculation and difficulty
    gameTime += deltaTime;
    
    // Update all game systems in proper order
    player->update(deltaTime);
    obstacleManager->update(deltaTime, gameTime);
    
    // Calculate current score
    currentScore = calculateScore();
    
    // Check for game-ending conditions
    if (checkCollisions()) {
        changeState(GameState::GAME_OVER);
    }
}

void Game::updateGameOverState(double deltaTime) {
    // In game over state, we might want to:
    // - Continue some animations
    // - Update UI effects
    // - Handle menu transitions
    // For now, we just maintain the current state
}

// ===== Game Logic Helper Methods =====

int Game::calculateScore() const {
    // Implement the same scoring formula as before
    double currentSpeed = obstacleManager->getCurrentSpeed();
    return static_cast<int>(gameTime * 10 + (currentSpeed / 200 - 1));
}

void Game::updateHighScore() {
    if (currentScore > highScore) {
        highScore = currentScore;
        std::cout << "New high score: " << highScore << std::endl;
    }
}

bool Game::checkCollisions() const {
    // Delegate collision detection to the specialized manager
    return CollisionManager::checkPlayerObstacleCollision(*player, *obstacleManager);
}

void Game::resetGame() {
    // Reset all game state for a fresh start
    gameTime = 0.0;
    currentScore = 0;
    
    // Reset all systems to initial state
    player->reset();
    obstacleManager->clear();
    
    std::cout << "Game reset to initial state" << std::endl;
}

// ===== UI Management Methods =====

void Game::updateScoreDisplays() {
    if (!fontLoaded) return;
    
    // Update score text with proper formatting
    scoreText.setString("Score: " + formatScore(currentScore));
    highScoreText.setString("High: " + formatScore(highScore));
}

void Game::renderUI() {
    if (!fontLoaded) return;
    
    // Always render score information
    window.draw(scoreText);
    window.draw(highScoreText);
    window.draw(instructionText);
    
    // Render state-specific UI elements
    switch (currentState) {
        case GameState::GAME_OVER:
            window.draw(gameOverText);
            break;
        default:
            // Other states can have their own UI elements
            break;
    }
}

// ===== Updated renderGameWorld method with enhanced rendering =====
void Game::renderGameWorld() {
    // Future: Render background sprite here
    // TextureManager& tm = TextureManager::getInstance();
    // sf::Sprite background = tm.createSprite(TextureManager::SpriteType::BACKGROUND);
    // window.draw(background);
    
    // Render all game world elements in proper order
    player->render(window);
    obstacleManager->render(window);
    
    // Future: Add particle effects, foreground elements, etc.
}

// ===== Performance and Utility Methods =====

void Game::maintainFrameRate() {
    // VSync and frame rate limiting handle most timing,
    // but we could add additional performance monitoring here
}

std::string Game::formatScore(int score) const {
    std::stringstream ss;
    ss << std::setfill('0') << std::setw(6) << score;
    return ss.str();
}

// ===== Updated logDebugInfo method with texture information =====
void Game::logDebugInfo() const {
    std::cout << "\n=== Game Debug Information ===" << std::endl;
    std::cout << "Game State: " << static_cast<int>(currentState) << std::endl;
    std::cout << "Score: " << currentScore << std::endl;
    std::cout << "Game Time: " << gameTime << "s" << std::endl;
    std::cout << "Player Position: (" << player->getPosX() << ", " << player->getPosY() << ")" << std::endl;
    std::cout << "Obstacles Count: " << obstacleManager->getObstacleCount() << std::endl;
    
    // Texture system debug info
    TextureManager& textureManager = TextureManager::getInstance();
    std::cout << "Loaded Textures: " << textureManager.getLoadedTextureCount() << std::endl;
    
    std::cout << "==============================\n" << std::endl;
}

// ===== Resource Management Helpers =====

bool Game::loadFont(const std::string& fontPath) {
    return gameFont.loadFromFile(fontPath);
}

void Game::configureText(sf::Text& text, const std::string& content, 
                        unsigned int size, const sf::Vector2f& position, 
                        const sf::Color& color) {
    text.setFont(gameFont);
    text.setString(content);
    text.setCharacterSize(size);
    text.setFillColor(color);
    text.setPosition(position);
}