#ifndef OBSTACLE_MANAGER_HPP
#define OBSTACLE_MANAGER_HPP

#include <SFML/Graphics.hpp>
#include <vector>
#include "Obstacle.hpp"

/**
 * ObstacleManager class: for managing obstacles lifetime in the game.
 * old version of the code was not well organized,
 * so I refactored it to make it more modular and easier to maintain.
 * Whole obstacle management(spawning, moving, deleting, hardness increasing) is capsulized here.
 */

class ObstacleManager {
private:
    // core member attributes - for obstacles whole management
    std::vector<Obstacle> obstacles; // Vector to hold all obstacles
    double spawnTimer;                  // Timer to control obstacle spawning
    double obstacleInterval;            // Time interval between obstacle spawns
    double obstacleSpeed;               // Speed of the obstacles

    // game balance and difficulty settings
    static const double INITIAL_SPAWN_INTERVAL; // Initial time interval for spawning obstacles (EASIEST)
    static const double MIN_SPAWN_INTERVAL;     // Minimum time interval for spawning obstacles (HARDEST)
    static const double INITIAL_OBSTACLE_SPEED; // Initial speed of the obstacles (EASIEST)
    static const double MAX_OBSTACLE_SPEED;     // Maximum speed of the obstacles (HARDEST)
    static const double SPAWN_POSITION_X;      // X position where obstacles spawn (right side of the screen)
    static const double SPAWN_POSITION_Y;      // Y position where obstacles spawn (ground level)

    static const double SPEED_INCREASE_RATE; // Rate at which obstacle speed increases over time
    static const double INTERVAL_DECREASE_RATE; // Rate at which spawn interval decreases over time


    // private helper methods
    void spawnObstacle();                               // make a new obstacle and add it to the vector
    void removeOffScreenObstacles();                    // delete obstacles that are off the screen from the vector
    void updateExistingObstacles(double deltaTime);      // update positions of existing obstacles based on their speed and time
    void applySpeedToAllObstacles();                    // whole obstacles speed update based on game time

public:
    ObstacleManager(); // Constructor to initialize obstacle manager
    ~ObstacleManager(); // Destructor

    // action methods
    void update(double deltaTime, double gameTime); // Update obstacles based on time
    void render(sf::RenderWindow& window); // Render all obstacles on the window
    void clear(); // Clear all obstacles

    // difficulty management methods
    void updateDifficulty(double gameTime); // Update difficulty based on game time

    // information getter method (const: for not changing inner data)
    const std::vector<Obstacle>& getObstacles() const;   // getting all obstacles in the vector for check collision or rendering
    size_t getObstacleCount() const;                     // obstacle on screen count (for debugging or UI display)
    double getCurrentSpeed() const;                      // get current speed (for calculating score)
    double getCurrentSpawnInterval() const;              // 현재 생성 간격 (for debugging or UI display)

    bool hasObstacles() const; // Check if there are any obstacles in the vector
    void getAverageObstacleDistance() const; // get average distance between obstacles (for checking difficulty)
};

#endif // OBSTACLE_MANAGER_HPP