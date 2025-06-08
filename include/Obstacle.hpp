#ifndef OBSTACLE_HPP
#define OBSTACLE_HPP
#include <SFML/Graphics.hpp>

class Obstacle {
private:
    sf::RectangleShape shape; // Obstacle shape
    double posX;                // X position of the obstacle
    double posY;                // Y position of the obstacle
    double velocityX;           // Horizontal velocity for obstacle movement
    
    /// @brief Initializes the obstacle shape with a random size and position.
    static const double DEFAULT_WIDTH;               // Width of the obstacle
    static const double DEFAULT_HEIGHT;              // Height of the obstacle
    static const double GROUND_Y;            // Default horizontal position of the obstacle

public:
    Obstacle(double startX, double startY, double moveSpeed); // Constructor to initialize obstacle at a specific position
    ~Obstacle(); // Destructor

    // 행동 메서드들
    void update(double deltaTime); // Update obstacle position based on velocity and time
    void render(sf::RenderWindow& window); // Render the obstacle on the window

    // 정보 제공 메서드들 (const: 데이터를 변경하지 않음을 보장)
    const sf::RectangleShape& getShape() const; // Get the shape of the obstacle
    double getPosX() const;     // Get X position of the obstacle
    double getPosY() const;     // Get Y position of the obstacle
    bool isOffScreen() const;   // Check if the obstacle is off the screen

    // option methods: manipulate obstacle status in game
    void setSpeed(double speed); // Set the horizontal speed of the obstacle
};


#endif // OBSTACLE_HPP// Obstacle class definition
// This class represents an obstacle in the game, which can be a rectangle or any other shape.