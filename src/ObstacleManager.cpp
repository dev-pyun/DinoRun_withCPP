#include "ObstacleManager.hpp"
#include <algorithm>  // for std::remove_if
#include <iostream>   // for debug output in getAverageObstacleDistance

// Static constant definitions - core game balance parameters
const double ObstacleManager::INITIAL_SPAWN_INTERVAL = 2.0;     // Start with 2 seconds between obstacles
const double ObstacleManager::MIN_SPAWN_INTERVAL = 0.8;        // Minimum 1 second at maximum difficulty
const double ObstacleManager::INITIAL_OBSTACLE_SPEED = 200.0;  // Starting speed: 200 pixels/second
const double ObstacleManager::MAX_OBSTACLE_SPEED = 400.0;      // Maximum speed: 400 pixels/second
const double ObstacleManager::SPAWN_POSITION_X = 800.0;        // Spawn at right edge of screen
const double ObstacleManager::SPAWN_POSITION_Y = 410.0;        // Spawn at ground level
const double ObstacleManager::SPEED_INCREASE_RATE = 5.0;       // Speed increases by 5 px/s every second
const double ObstacleManager::INTERVAL_DECREASE_RATE = 0.03;   // Interval decreases by 0.01s every second

// Constructor: Initialize ObstacleManager with default values
ObstacleManager::ObstacleManager() 
    : spawnTimer(0.0), 
      obstacleInterval(INITIAL_SPAWN_INTERVAL),
      obstacleSpeed(INITIAL_OBSTACLE_SPEED) {
    
    // Vector is automatically initialized as empty
    // All other member variables are set via initialization list above
}

// Destructor: Clean up resources (automatic cleanup for std::vector)
ObstacleManager::~ObstacleManager() {
    // std::vector<Obstacle> automatically handles cleanup
    // Each Obstacle object will also have its destructor called automatically
}

// Main update method: Called every frame to manage all obstacle-related logic
void ObstacleManager::update(double deltaTime, double gameTime) {
    // Step 1: Update difficulty based on elapsed game time
    updateDifficulty(gameTime);
    
    // Step 2: Update positions of all existing obstacles
    updateExistingObstacles(deltaTime);
    
    // Step 3: Check if it's time to spawn a new obstacle
    spawnTimer += deltaTime;
    if (spawnTimer >= obstacleInterval) {
        spawnObstacle();
        spawnTimer = 0.0;  // Reset spawn timer
    }
    
    // Step 4: Remove obstacles that have moved off-screen
    removeOffScreenObstacles();
}

// Render method: Draw all obstacles to the screen
void ObstacleManager::render(sf::RenderWindow& window) {
    // Iterate through all obstacles and call their individual render methods
    for (auto& obstacle : obstacles) {
        obstacle.render(window);
    }
}

// Clear method: Reset manager to initial state (used when restarting game)
void ObstacleManager::clear() {
    obstacles.clear();                          // Remove all obstacles from vector
    spawnTimer = 0.0;                          // Reset spawn timer
    obstacleInterval = INITIAL_SPAWN_INTERVAL; // Reset spawn interval to initial value
    obstacleSpeed = INITIAL_OBSTACLE_SPEED;    // Reset speed to initial value
}

// Difficulty update method: Progressively increase challenge based on game time
void ObstacleManager::updateDifficulty(double gameTime) {
    // Calculate new obstacle speed: base speed + (time * increase rate)
    // Example: 0s=200, 20s=300, 40s=400 (capped at maximum)
    obstacleSpeed = INITIAL_OBSTACLE_SPEED + (gameTime * SPEED_INCREASE_RATE);
    if (obstacleSpeed > MAX_OBSTACLE_SPEED) {
        obstacleSpeed = MAX_OBSTACLE_SPEED;  // Cap at maximum speed
    }
    
    // Calculate new spawn interval: base interval - (time * decrease rate)
    // Example: 0s=2.0s, 50s=1.5s, 100s=1.0s (capped at minimum)
    obstacleInterval = INITIAL_SPAWN_INTERVAL - (gameTime * INTERVAL_DECREASE_RATE);
    if (obstacleInterval < MIN_SPAWN_INTERVAL) {
        obstacleInterval = MIN_SPAWN_INTERVAL;  // Cap at minimum interval
    }
}

// Getter methods: Provide read-only access to internal state
const std::vector<Obstacle>& ObstacleManager::getObstacles() const {
    return obstacles;
}

size_t ObstacleManager::getObstacleCount() const {
    return obstacles.size();
}

double ObstacleManager::getCurrentSpeed() const {
    return obstacleSpeed;
}

double ObstacleManager::getCurrentSpawnInterval() const {
    return obstacleInterval;
}

bool ObstacleManager::hasObstacles() const {
    return !obstacles.empty();
}

// Debug method: Calculate and output average distance between obstacles
void ObstacleManager::getAverageObstacleDistance() const {
    if (obstacles.size() < 2) {
        std::cout << "Average obstacle distance: N/A (less than 2 obstacles)" << std::endl;
        return;
    }
    
    double totalDistance = 0.0;
    for (size_t i = 1; i < obstacles.size(); ++i) {
        // Calculate distance between consecutive obstacles
        double distance = obstacles[i-1].getPosX() - obstacles[i].getPosX();
        totalDistance += distance;
    }
    
    double averageDistance = totalDistance / (obstacles.size() - 1);
    std::cout << "Average obstacle distance: " << averageDistance << " pixels" << std::endl;
}

// ========== Private Helper Methods ==========

// spawnObstacle: Create a new obstacle and add it to the vector
void ObstacleManager::spawnObstacle() {
    // Use emplace_back for efficient in-place construction
    // This creates the Obstacle object directly in the vector without copying
    obstacles.emplace_back(SPAWN_POSITION_X, SPAWN_POSITION_Y, obstacleSpeed);
}

// removeOffScreenObstacles: Efficiently remove obstacles that are no longer visible
void ObstacleManager::removeOffScreenObstacles() {
    // Use erase-remove idiom: C++ standard pattern for efficient element removal
    // Lambda function checks if each obstacle is off-screen
    obstacles.erase(
        std::remove_if(obstacles.begin(), obstacles.end(),
            [](const Obstacle& obs) { return obs.isOffScreen(); }),
        obstacles.end()
    );
}

// updateExistingObstacles: Update position and speed of all current obstacles
void ObstacleManager::updateExistingObstacles(double deltaTime) {
    for (auto& obstacle : obstacles) {
        // Apply current speed to obstacle (important for difficulty scaling)
        obstacle.setSpeed(obstacleSpeed);
        
        // Update obstacle position based on its speed and elapsed time
        obstacle.update(deltaTime);
    }
}

// applySpeedToAllObstacles: Synchronize all obstacles with current speed
void ObstacleManager::applySpeedToAllObstacles() {
    // This method ensures all obstacles move at the current difficulty-adjusted speed
    for (auto& obstacle : obstacles) {
        obstacle.setSpeed(obstacleSpeed);
    }
}