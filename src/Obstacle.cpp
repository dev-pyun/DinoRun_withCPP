#include "Obstacle.hpp"

const double Obstacle::DEFAULT_WIDTH = 30.0;    // Default width of the obstacle
const double Obstacle::DEFAULT_HEIGHT = 50.0;   // Default height of the obstacle
const double Obstacle::GROUND_Y = 400.0;        // Default horizontal position of the obstacle

// Constructor to initialize obstacle at a specific position
Obstacle::Obstacle(double startX, double startY, double moveSpeed)
    : posX(startX), posY(startY), velocityX(moveSpeed) {
        // Initialize the obstacle shape
    shape.setSize(sf::Vector2f(DEFAULT_WIDTH, DEFAULT_HEIGHT)); // Set obstacle size
    shape.setFillColor(sf::Color::Red); // Set obstacle color
    shape.setOutlineColor(sf::Color::Black); // Set outline color
    shape.setOutlineThickness(2); // Set outline thickness

    // Set the initial position of the obstacle
    shape.setPosition(posX, posY); 
}

// Destructor
Obstacle::~Obstacle() {
    // Destructor does not need to do anything special cuz this is SFML!
}


// Update obstacle position based on velocity and time
void Obstacle::update(double deltaTime) {
    // Move the obstacle left based on its velocity and the time elapsed
    posX -= velocityX * deltaTime;

    // Update the shape position
    shape.setPosition(posX, posY); 
}

// Render the obstacle on the window
void Obstacle::render(sf::RenderWindow& window) {
    window.draw(shape); // Draw the obstacle shape on the window
}

// Get the shape of the obstacle
const sf::RectangleShape& Obstacle::getShape() const {
    return shape; // Return the obstacle shape
}

// Get X position of the obstacle
double Obstacle::getPosX() const {
    return posX; // Return the X position of the obstacle
}

// Get Y position of the obstacle
double Obstacle::getPosY() const {
    return posY; // Return the Y position of the obstacle
}

// Check if the obstacle is off the screen
bool Obstacle::isOffScreen() const {
    // Check if the obstacle's X position is less than -50 (off the left side of the screen)
    return posX < -50; 
}

// Set the horizontal speed of the obstacle
void Obstacle::setSpeed(double newSpeed) {
    velocityX = newSpeed; // Set the horizontal speed of the obstacle
}