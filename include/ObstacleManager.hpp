#ifndef OBSTACLE_MANAGER_HPP
#define OBSTACLE_MANAGER_HPP

#include <SFML/Graphics.hpp>
#include <vector>
#include <random>
#include <unordered_map>
#include "Obstacle.hpp"

/**
 * ObstacleManager class: for managing obstacles lifetime in the game.
 * old version of the code was not well organized,
 * so I refactored it to make it more modular and easier to maintain.
 * Whole obstacle management(spawning, moving, deleting, hardness increasing) is capsulized here.
 */

class ObstacleManager {
public:
    /**
     * Enumeration for different obstacle patterns
     * Each pattern creates a specific challenge scenario for the player
     */
    enum class ObstaclePattern {
        SINGLE_SMALL,        // Single small cactus (easiest)
        SINGLE_MID,          // Single medium cactus 
        SINGLE_LARGE,        // Single large cactus (medium difficulty)
        DOUBLE_CLOSE,        // Two obstacles close together (hard)
        TRIPLE_CLUSTER,      // Three obstacles clustered (very hard)
        MIXED_HEIGHTS,       // Mixed obstacle types (challenging)
        WIDE_GAP,           // Obstacles with wide gaps (easy breather)
        TIGHT_SEQUENCE      // Rapid sequence of obstacles (expert level)
    };
    
    /**
     * Structure defining a specific obstacle spawn pattern
     * Contains all necessary information for spawning coordinated obstacle groups
     */
    struct PatternDefinition {
        std::vector<Obstacle::ObstacleType> obstacleTypes;  // Types of obstacles to spawn
        std::vector<double> relativePositions;             // X offsets from base spawn position
        double patternDifficulty;                           // Difficulty rating (0.0 - 1.0)
        double minGameTime;                                 // Minimum game time to unlock this pattern
        std::string patternName;                            // Human-readable name for debugging
        
        PatternDefinition() : patternDifficulty(0.0), minGameTime(0.0), patternName("Unknown") {}
    };
private:
    // core member attributes - for obstacles whole management
    std::vector<Obstacle> obstacles; // Vector to hold all obstacles
    double spawnTimer;                  // Timer to control obstacle spawning
    double obstacleInterval;            // Time interval between obstacle spawns
    double obstacleSpeed;               // Speed of the obstacles

    // ===== Pattern System =====
    std::unordered_map<ObstaclePattern, PatternDefinition> patternDefinitions;
    std::vector<ObstaclePattern> availablePatterns;
    ObstaclePattern lastPattern;
    int consecutiveHardPatterns;
    
    // ===== Random Generation =====
    mutable std::mt19937 randomGenerator;
    mutable std::uniform_real_distribution<double> uniformDist;
    
    // ===== Difficulty Scaling =====
    double currentDifficulty;              // Current difficulty level (0.0 - 1.0)
    double patternCooldownTimer;           // Time since last complex pattern
    
    // game balance and difficulty settings
    static const double INITIAL_SPAWN_INTERVAL; // Initial time interval for spawning obstacles (EASIEST)
    static const double MIN_SPAWN_INTERVAL;     // Minimum time interval for spawning obstacles (HARDEST)
    static const double INITIAL_OBSTACLE_SPEED; // Initial speed of the obstacles (EASIEST)
    static const double MAX_OBSTACLE_SPEED;     // Maximum speed of the obstacles (HARDEST)
    static const double SPAWN_POSITION_X;      // X position where obstacles spawn (right side of the screen)
    static const double SPAWN_POSITION_Y;      // Y position where obstacles spawn (ground level)

    static const double SPEED_INCREASE_RATE; // Rate at which obstacle speed increases over time
    static const double INTERVAL_DECREASE_RATE; // Rate at which spawn interval decreases over time


    // ===== Pattern System Constants =====
    static const double PATTERN_COOLDOWN_TIME;     // Minimum time between complex patterns
    static const double DIFFICULTY_INCREASE_RATE;  // Rate of difficulty progression
    static const int MAX_CONSECUTIVE_HARD;         // Maximum consecutive hard patterns

    // ===== Pattern System Implementation =====
    /**
     * Initialize all obstacle patterns with their definitions
     * Sets up timing, positioning, and difficulty for each pattern type
     */
    void initializePatterns();
    
    /**
     * Update list of available patterns based on current game time
     * Progressively unlocks more challenging patterns as game progresses
     * 
     * @param gameTime Current game time
     */
    void updateAvailablePatterns(double gameTime);
    
    /**
     * Calculate weighted random selection for patterns
     * Biases selection toward patterns appropriate for current difficulty
     * 
     * @param gameTime Current game time
     * @return Selected pattern based on weighted probability
     */
    ObstaclePattern weightedPatternSelection(double gameTime) const;
    
    /**
     * Check if a pattern should be avoided due to cooldown or consecutive limits
     * Prevents overwhelming the player with too many difficult patterns
     * 
     * @param pattern Pattern to evaluate
     * @return true if pattern should be avoided this cycle
     */
    bool shouldAvoidPattern(ObstaclePattern pattern) const;
    
    /**
     * Get pattern difficulty weight for selection algorithm
     * Higher weight = higher selection probability at appropriate difficulty
     * 
     * @param pattern Pattern to evaluate
     * @param currentDifficulty Current game difficulty (0.0 - 1.0)
     * @return Weight for weighted random selection
     */
    double getPatternWeight(ObstaclePattern pattern, double currentDifficulty) const;

    // ===== Obstacle Spawning Helpers =====
    void spawnSingleObstacle(Obstacle::ObstacleType type, double xOffset = 0.0);

    // private helper methods
    void spawnObstacle();                               // make a new obstacle and add it to the vector
    void removeOffScreenObstacles();                    // delete obstacles that are off the screen from the vector
    void updateExistingObstacles(double deltaTime);      // update positions of existing obstacles based on their speed and time
    void applySpeedToAllObstacles();                    // whole obstacles speed update based on game time

    // ===== Utility Methods =====
    double generateRandomOffset(double min, double max) const;
    Obstacle::ObstacleType getRandomObstacleType() const;

public:
    ObstacleManager(); // Constructor to initialize obstacle manager
    ~ObstacleManager(); // Destructor

    // action methods
    void update(double deltaTime, double gameTime); // Update obstacles based on time
    void render(sf::RenderWindow& window); // Render all obstacles on the window
    void clear(); // Clear all obstacles

    // difficulty management methods
    void updateDifficulty(double gameTime); // Update difficulty based on game time
    double getCurrentDifficulty() const;    // Get current difficulty level (0.0 - 1.0)
    
    // information getter method (const: for not changing inner data)
    const std::vector<Obstacle>& getObstacles() const;   // getting all obstacles in the vector for check collision or rendering
    size_t getObstacleCount() const;                     // obstacle on screen count (for debugging or UI display)
    double getCurrentSpeed() const;                      // get current speed (for calculating score)
    double getCurrentSpawnInterval() const;              // 현재 생성 간격 (for debugging or UI display)

    bool hasObstacles() const; // Check if there are any obstacles in the vector
    void getAverageObstacleDistance() const; // get average distance between obstacles (for checking difficulty)

    // ===== Pattern System Methods =====
    
    /**
     * Select next obstacle pattern based on current difficulty and game state
     * Uses weighted random selection to ensure appropriate challenge level
     * 
     * @param gameTime Current game time for pattern unlocking
     * @return Selected obstacle pattern to spawn
     */
    ObstaclePattern selectPattern(double gameTime);
    
    /**
     * Spawn obstacles according to specified pattern
     * Creates multiple obstacles with coordinated timing and positioning
     * 
     * @param pattern Pattern type to spawn
     */
    void spawnPattern(ObstaclePattern pattern);
    
    /**
     * Get current pattern name for debugging and UI display
     * 
     * @return Human-readable name of last spawned pattern
     */
    std::string getCurrentPatternName() const;
};

#endif // OBSTACLE_MANAGER_HPP