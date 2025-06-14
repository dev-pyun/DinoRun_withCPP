#include "Obstacle.hpp"
#include <iostream>
#include <random>

// ===== Static Constants =====
const double Obstacle::GROUND_Y = 400.0;

// Define sizes for different obstacle types (based on sprite dimensions)
const sf::Vector2f Obstacle::SMALL_CACTUS_SIZE = sf::Vector2f(15.0f, 35.0f);
const sf::Vector2f Obstacle::MID_CACTUS_SIZE = sf::Vector2f(25.0f, 48.0f);
const sf::Vector2f Obstacle::LARGE_CACTUS_SIZE = sf::Vector2f(32.0f, 68.0f);
const sf::Vector2f Obstacle::CLUSTER_CACTUS_SIZE = sf::Vector2f(40.0f, 35.0f);  // Future use

// Define collision sizes for different obstacle types
const sf::Vector2f Obstacle::SMALL_CACTUS_COLLISION_SIZE = sf::Vector2f(1.0f, 35.0f); 
const sf::Vector2f Obstacle::MID_CACTUS_COLLISION_SIZE = sf::Vector2f(1.5f, 48.0f);
const sf::Vector2f Obstacle::LARGE_CACTUS_COLLISION_SIZE = sf::Vector2f(2.0f, 68.0f);
const sf::Vector2f Obstacle::CLUSTER_CACTUS_COLLISION_SIZE = sf::Vector2f(32.0f, 35.0f);  // Future use

// ===== Constructors and Destructor =====

Obstacle::Obstacle(double startX, double startY, double moveSpeed)
    : posX(startX), posY(startY), velocityX(moveSpeed) {
    
    // Generate random obstacle type for variety
    obstacleType = generateRandomType();
    
    // Initialize sprite system
    initializeSprite();
    
    std::cout << "Obstacle created at (" << startX << ", " << startY 
              << ") with type " << static_cast<int>(obstacleType) << std::endl;
}

Obstacle::Obstacle(double startX, double startY, double moveSpeed, ObstacleType type)
    : posX(startX), posY(startY), velocityX(moveSpeed), obstacleType(type) {
    
    // Initialize sprite system with specified type
    initializeSprite();
    
    std::cout << "Obstacle created at (" << startX << ", " << startY 
              << ") with specified type " << static_cast<int>(type) << std::endl;
}

Obstacle::~Obstacle() {
    // Sprite and bounding box cleanup is automatic
}

// ===== Core Action Methods =====

void Obstacle::update(double deltaTime) {
    // Move obstacle to the left
    posX -= velocityX * deltaTime;
    
    // Update sprite position
    currentSprite.setPosition(static_cast<float>(posX), static_cast<float>(posY));
    
    // Update collision bounding box
    updateBoundingBox();
}

void Obstacle::render(sf::RenderWindow& window) {
    // Render the sprite
    window.draw(currentSprite);
    
    // Optionally render debug bounding box (comment out for release)
    window.draw(boundingBox);
}

// ===== Information Methods =====

const sf::RectangleShape& Obstacle::getShape() const {
    return boundingBox;
}

const sf::Sprite& Obstacle::getSprite() const {
    return currentSprite;
}

double Obstacle::getPosX() const {
    return posX;
}

double Obstacle::getPosY() const {
    return posY;
}

Obstacle::ObstacleType Obstacle::getObstacleType() const {
    return obstacleType;
}

sf::Vector2f Obstacle::getSize() const {
    return currentSize;
}

bool Obstacle::isOffScreen() const {
    // Check if obstacle's right edge is off the left side of screen
    return (posX + currentSize.x) < -50.0;
}

// ===== Manipulation Methods =====

void Obstacle::setSpeed(double newSpeed) {
    velocityX = newSpeed;
}

void Obstacle::changeType(ObstacleType newType) {
    if (obstacleType != newType) {
        obstacleType = newType;
        
        // Update sprite and size for new type
        applySpriteForType();
        updateBoundingBox();
        
        std::cout << "Obstacle type changed to " << static_cast<int>(newType) << std::endl;
    }
}

// ===== Private Helper Methods =====

void Obstacle::initializeSprite() {
    // Get size for the obstacle type
    currentSize = getSizeForType(obstacleType);
    
    // Set up bounding box for collision detection
    boundingBox.setSize(currentSize);
    boundingBox.setFillColor(sf::Color::Transparent);  // Invisible
    boundingBox.setOutlineColor(sf::Color::Red);      // Debug outline (can be removed)
    boundingBox.setOutlineThickness(2.0f);             // Debug outline (can be removed)
    
    // Apply sprite for the current type
    applySpriteForType();
    
    // Set initial positions
    currentSprite.setPosition(static_cast<float>(posX), static_cast<float>(posY));
    updateBoundingBox();
    
    std::cout << "Obstacle sprite system initialized for type " 
              << static_cast<int>(obstacleType) << std::endl;
}

void Obstacle::applySpriteForType() {
    // Get the appropriate sprite type for texture manager
    spriteType = getSpriteTypeFromObstacleType(obstacleType);
    
    // Get texture manager instance
    TextureManager& textureManager = TextureManager::getInstance();
    
    // Create sprite with current size
    currentSprite = textureManager.createSprite(spriteType, currentSize);
    
    // Position the sprite
    currentSprite.setPosition(static_cast<float>(posX), static_cast<float>(posY));
}

void Obstacle::updateBoundingBox() {
    // collision box at the center of sprite
    sf::Vector2f collisionSize = getCollisionSizeForType(obstacleType);
    float offsetX = (currentSize.x - collisionSize.x) / 2.0f;
    float offsetY = (currentSize.y - collisionSize.y) / 2.0f;
    boundingBox.setPosition(posX + offsetX, posY + offsetY);
    boundingBox.setSize(collisionSize);
}

TextureManager::SpriteType Obstacle::getSpriteTypeFromObstacleType(ObstacleType type) const {
    switch (type) {
        case ObstacleType::CACTUS_SMALL:
            return TextureManager::SpriteType::CACTUS_SMALL;
        
        case ObstacleType::CACTUS_MID:
            return TextureManager::SpriteType::CACTUS_MID;    
        
        case ObstacleType::CACTUS_LARGE:
            return TextureManager::SpriteType::CACTUS_LARGE;
            
        case ObstacleType::CACTUS_CLUSTER:
            // For now, use small cactus until cluster sprite is available
            return TextureManager::SpriteType::CACTUS_SMALL;
            
        default:
            return TextureManager::SpriteType::CACTUS_SMALL;
    }
}

sf::Vector2f Obstacle::getSizeForType(ObstacleType type) const {
    switch (type) {
        case ObstacleType::CACTUS_SMALL:
            return SMALL_CACTUS_SIZE;
        
        case ObstacleType::CACTUS_MID:
            return MID_CACTUS_SIZE;
            
        case ObstacleType::CACTUS_LARGE:
            return LARGE_CACTUS_SIZE;
            
        case ObstacleType::CACTUS_CLUSTER:
            return CLUSTER_CACTUS_SIZE;
            
        default:
            return SMALL_CACTUS_SIZE;
    }
}

Obstacle::ObstacleType Obstacle::generateRandomType() {
    // Create random number generator
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(0, 100);
    
    int randomValue = dis(gen);
    
    // Weighted distribution for gameplay balance
    // 60% small cactus (easier), 40% large cactus (harder)
    if (randomValue < 40) {
        return ObstacleType::CACTUS_SMALL;
    } else if (randomValue < 80) {
        return ObstacleType::CACTUS_MID;
    } else {
        return ObstacleType::CACTUS_LARGE;
    }
    
    // Future: can add CACTUS_CLUSTER with lower probability
    // else if (randomValue < 95) {
    //     return ObstacleType::CACTUS_LARGE;
    // } else {
    //     return ObstacleType::CACTUS_CLUSTER;
    // }
}

sf::Vector2f Obstacle::getCollisionSizeForType(ObstacleType type) const {
    switch (type) {
        case ObstacleType::CACTUS_SMALL:
            return SMALL_CACTUS_COLLISION_SIZE;
        case ObstacleType::CACTUS_MID:
            return MID_CACTUS_COLLISION_SIZE;
        case ObstacleType::CACTUS_LARGE:
            return LARGE_CACTUS_COLLISION_SIZE;
        case ObstacleType::CACTUS_CLUSTER:
            return CLUSTER_CACTUS_COLLISION_SIZE;
        default:
            return SMALL_CACTUS_COLLISION_SIZE;
    }
}