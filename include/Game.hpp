#ifndef GAME_HPP
#define GAME_HPP

#include <SFML/Graphics.hpp>
#include <memory>
#include <sstream>
#include <iomanip>

// Forward declarations for our game systems
class Player;
class ObstacleManager;
class CollisionManager;
class TextureManager;

/**
 * Game class: The ultimate orchestrator of the entire game system
 * 
 * Design Philosophy:
 * This class represents the culmination of object-oriented design principles,
 * serving as the conductor of an orchestra where each system (Player, ObstacleManager, etc.)
 * plays its part while Game ensures perfect harmony and timing.
 * 
 * Key Responsibilities:
 * 1. System Lifecycle Management - Creating, updating, and destroying game systems
 * 2. State Management - Handling different game states (menu, playing, paused, game over)
 * 3. Resource Management - Centralized loading and management of fonts, textures, sounds
 * 4. Flow Control - Coordinating the interaction between different systems
 * 5. Performance Management - Ensuring consistent frame rate and smooth gameplay
 * 
 * This class implements the "Inversion of Control" principle, where instead of
 * individual systems managing their own lifecycle, Game manages when and how
 * each system should operate, leading to better coordination and fewer bugs.
 */
class Game {
public:
    /**
     * Game state enumeration using modern C++ enum class for type safety
     * This replaces the simple bool gameOver with a more sophisticated state system
     */
    enum class GameState {
        MENU,        // Main menu (future expansion)
        PLAYING,     // Active gameplay
        PAUSED,      // Game temporarily stopped (future expansion)
        GAME_OVER,   // Player failed, showing restart option
        SETTINGS     // Configuration screen (future expansion)
    };

private:
    // ===== Core Engine Layer =====
    sf::RenderWindow window;           // The main game window
    sf::Clock frameClock;             // For consistent frame timing
    sf::Event currentEvent;           // Current SFML event being processed
    
    // ===== Game Systems Layer =====
    std::unique_ptr<Player> player;              // Smart pointer for automatic memory management
    std::unique_ptr<ObstacleManager> obstacleManager;  // Managed obstacle system
    
    // ===== State Management Layer =====
    GameState currentState;           // Current game state
    GameState previousState;          // For state transition handling
    
    // ===== Game Logic Layer =====
    double gameTime;                  // Total elapsed game time
    int currentScore;                 // Player's current score
    int highScore;                    // Session's highest score
    bool isRunning;                   // Master control flag for game loop
    
    // ===== Resource Management Layer =====
    sf::Font gameFont;                // Main font for all text rendering
    bool fontLoaded;                  // Font loading status flag
    
    // ===== UI Text Elements =====
    sf::Text gameOverText;            // Game over message
    sf::Text scoreText;               // Current score display
    sf::Text highScoreText;           // High score display
    sf::Text instructionText;         // Control instructions
    
    // ===== Configuration Constants =====
    static const int WINDOW_WIDTH;    // Game window dimensions
    static const int WINDOW_HEIGHT;
    static const int TARGET_FPS;      // Target frame rate for consistent gameplay
    static const std::string WINDOW_TITLE;  // Game window title

public:
    // ===== Core Lifecycle Methods =====
    
    /**
     * Constructor: Initialize the Game with default settings
     * Sets up the window, initializes all systems, loads resources
     */
    Game();
    
    /**
     * Destructor: Clean up all resources and systems
     * Smart pointers handle most cleanup automatically, but we ensure clean shutdown
     */
    ~Game();
    
    /**
     * Main game execution method - the single entry point for the entire game
     * This method contains the complete game loop and handles all high-level coordination
     * 
     * Design: This method should be the ONLY public method that external code calls
     * Everything else is internal implementation managed by the Game class itself
     * 
     * @return int Exit code (0 for normal termination)
     */
    int run();

private:
    // ===== Initialization Methods =====
    
    /**
     * Initialize the SFML window with proper settings
     * Configures window properties, VSync, frame rate limiting, etc.
     */
    void initializeWindow();
    
    /**
     * Initialize the texture management system
     * Must be called before creating any game objects that use sprites
     */
    void initializeTextureSystem();

    /**
     * Initialize all game systems (Player, ObstacleManager, etc.)
     * Creates system objects and establishes their initial states
     */
    void initializeSystems();
    
    /**
     * Load all game resources (fonts, textures, sounds)
     * Centralizes resource loading with proper error handling
     */
    void loadResources();
    
    /**
     * Initialize all UI text elements with proper fonts and positioning
     * Sets up score displays, game over messages, instructions, etc.
     */
    void initializeUI();
    
    // ===== Core Game Loop Methods =====
    
    /**
     * Process all SFML events (keyboard, mouse, window events)
     * Handles input differently based on current game state
     */
    void handleEvents();
    
    /**
     * Update all game systems based on current state
     * Coordinates the update sequence and timing for all systems
     * 
     * @param deltaTime Time elapsed since last frame
     */
    void update(double deltaTime);
    
    /**
     * Render all game elements to the screen
     * Manages the complete rendering pipeline in correct order
     */
    void render();
    
    // ===== State Management Methods =====
    
    /**
     * Transition to a new game state with proper cleanup and initialization
     * Handles state-specific setup and teardown operations
     * 
     * @param newState The target game state to transition to
     */
    void changeState(GameState newState);
    
    /**
     * Handle input events specific to the PLAYING state
     * Processes jump commands and game-specific controls
     */
    void handlePlayingStateEvents();
    
    /**
     * Handle input events specific to the GAME_OVER state
     * Processes restart commands and menu navigation
     */
    void handleGameOverStateEvents();
    
    /**
     * Update game logic when in PLAYING state
     * Coordinates all active gameplay systems
     * 
     * @param deltaTime Time elapsed since last frame
     */
    void updatePlayingState(double deltaTime);
    
    /**
     * Update logic when in GAME_OVER state
     * Handles score finalization and UI updates
     * 
     * @param deltaTime Time elapsed since last frame
     */
    void updateGameOverState(double deltaTime);
    
    // ===== Game Logic Helper Methods =====
    
    /**
     * Calculate current score based on game time and difficulty
     * Implements the scoring algorithm with speed bonuses
     * 
     * @return Current calculated score
     */
    int calculateScore() const;
    
    /**
     * Update high score if current score exceeds it
     * Handles score comparison and storage
     */
    void updateHighScore();
    
    /**
     * Check for collisions between all relevant game objects
     * Coordinates collision detection through CollisionManager
     * 
     * @return true if any collision detected, false otherwise
     */
    bool checkCollisions() const;
    
    /**
     * Reset all game systems to initial state for new game
     * Coordinates the reset process across all systems
     */
    void resetGame();
    
    // ===== UI Management Methods =====
    
    /**
     * Update all score display texts with current values
     * Handles text formatting and positioning
     */
    void updateScoreDisplays();
    
    /**
     * Render UI elements appropriate for current game state
     * State-specific UI rendering with proper layering
     */
    void renderUI();
    
    /**
     * Render game world elements (player, obstacles, effects)
     * Coordinates rendering of all gameplay visuals
     */
    void renderGameWorld();
    
    // ===== Performance and Utility Methods =====
    
    /**
     * Maintain consistent frame rate through proper timing
     * Implements frame rate limiting and VSync coordination
     */
    void maintainFrameRate();
    
    /**
     * Get formatted string representation of a score
     * Provides consistent score formatting across the game
     * 
     * @param score The score value to format
     * @return Formatted score string (e.g., "000123")
     */
    std::string formatScore(int score) const;
    
    /**
     * Log debug information about current game state
     * Useful for development and debugging (can be disabled in release)
     */
    void logDebugInfo() const;
    
    // ===== Resource Management Helpers =====
    
    /**
     * Safely load a font with error handling
     * Attempts multiple font paths and provides fallback options
     * 
     * @param fontPath Primary path to font file
     * @return true if font loaded successfully, false otherwise
     */
    bool loadFont(const std::string& fontPath);
    
    /**
     * Configure text object with standard game styling
     * Applies consistent font, size, and color across UI elements
     * 
     * @param text Reference to text object to configure
     * @param content String content for the text
     * @param size Character size
     * @param position Position on screen
     * @param color Text color (defaults to black)
     */
    void configureText(sf::Text& text, const std::string& content, 
                      unsigned int size, const sf::Vector2f& position, 
                      const sf::Color& color = sf::Color::Black);

    /**
     * Reload textures (useful for resolution changes or asset updates)
     * 
     * @return true if reload successful
     */
    bool reloadTextures();

    /**
     * Get texture memory usage information
     * Useful for performance monitoring
     * 
     * @return Estimated memory usage in bytes
     */
    size_t getTextureMemoryUsage() const;

private:
    // ===== Deleted Methods for Safety =====
    // Prevent copying and assignment to ensure single Game instance
    Game(const Game&) = delete;
    Game& operator=(const Game&) = delete;
    Game(Game&&) = delete;
    Game& operator=(Game&&) = delete;
};

#endif // GAME_HPP