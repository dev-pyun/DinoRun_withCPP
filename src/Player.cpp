#include "Player.hpp"

const double Player::GROUND_Y = 400.0; // Ground level Y coordinate
const double Player::JUMP_STRENGTH = -400.0; // Initial jump velocity
const double Player::GRAVITY = 700.0; // Gravity acceleration

Player::Player(float startX, float startY) 
    : posX(startX), posY(startY), velocityY(0.0), isJumping(false) {
    shape.setSize(sf::Vector2f(40, 60)); // Set player size
    shape.setFillColor(sf::Color::Green); // Set player color
    shape.setOutlineColor(sf::Color::Black); // Set outline color
    shape.setOutlineThickness(2); // Set outline thickness

    // set initial position
    shape.setPosition(posX, posY);
}

Player::~Player() {
    // Destructor does not need to do anything special
} 

void Player::jump() {
    if (!isJumping) {
        velocityY = JUMP_STRENGTH; // Set initial jump velocity
        isJumping = true; // Set jumping status
    }
}

void Player::update(float deltaTime) {
    if (isJumping) {
        // Apply gravity
        velocityY += GRAVITY * deltaTime;
        posY += velocityY * deltaTime;

        // Check if player has landed
        if (posY >= GROUND_Y) {
            posY = GROUND_Y; // Reset position to ground level
            velocityY = 0; // Reset vertical velocity
            isJumping = false; // Reset jumping status
        }
    }

    // Update player shape position
    shape.setPosition(posX, posY);
}

void Player::render(sf::RenderWindow& window) {
    window.draw(shape); // Draw player shape on the window
}

void Player::reset() {
    posX = 100.0; // Reset to initial X position
    posY = GROUND_Y; // Reset to ground level
    velocityY = 0.0; // Reset vertical velocity
    isJumping = false; // Reset jumping status

    shape.setPosition(posX, posY); // Update shape position
}

const sf::RectangleShape& Player::getShape() const {
    return shape; // Return player shape
}

bool Player::getIsJumping() const {
    return isJumping; // Return jumping status
}

double Player::getPosX() const {
    return posX; // Return X position
}

double Player::getPosY() const {
    return posY; // Return Y position
}