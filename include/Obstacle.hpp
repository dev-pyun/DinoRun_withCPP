#ifndef OBSTACLE_HPP
#define OBSTACLE_HPP

#include <SFML/Graphics.hpp>
#include "TextureManager.hpp"

/**
 * Obstacle class: Enhanced with sprite-based rendering and variety
 * 
 * Major improvements in this version:
 * - Support for different obstacle types (small cactus, large cactus)
 * - Sprite-based rendering instead of colored rectangles
 * - Automatic sprite selection based on obstacle type
 * - Maintains collision system compatibility
 * - Random obstacle type generation for variety
 */
class Obstacle {
public:
    /**
     * Enum for different types of obstacles
     * Determines which sprite to use and obstacle properties
     */
    enum class ObstacleType {
        CACTUS_SMALL,      // Small cactus (easier to jump over)
        CACTUS_MID,      // MID cactus (easier to jump over)
        CACTUS_LARGE,      // Large cactus (requires precise timing)
        CACTUS_CLUSTER     // Future: cluster of small cacti
    };

private:
    // ===== Sprite System =====
    sf::Sprite currentSprite;
    TextureManager::SpriteType spriteType;
    ObstacleType obstacleType;
    
    // ===== Position and Movement =====
    double posX;
    double posY;
    double velocityX;
    
    // ===== Size Management =====
    sf::Vector2f currentSize;
    sf::RectangleShape boundingBox;  // For collision detection (invisible)
    
    // ===== Type-Specific Constants =====
    static const double GROUND_Y;
    
    // Size definitions for different obstacle types
    static const sf::Vector2f SMALL_CACTUS_SIZE;
    static const sf::Vector2f MID_CACTUS_SIZE;
    static const sf::Vector2f LARGE_CACTUS_SIZE;
    static const sf::Vector2f CLUSTER_CACTUS_SIZE;
    
    // for the better collision system : DIVIDE SPRITE AND COLLISION
    static const sf::Vector2f SMALL_CACTUS_COLLISION_SIZE;  
    static const sf::Vector2f MID_CACTUS_COLLISION_SIZE;
    static const sf::Vector2f LARGE_CACTUS_COLLISION_SIZE;  
    static const sf::Vector2f CLUSTER_CACTUS_COLLISION_SIZE;

public:
    /**
     * Constructor: Create obstacle with random type
     * 
     * @param startX Initial X position (usually off-screen right)
     * @param startY Initial Y position (usually ground level)
     * @param moveSpeed Horizontal movement speed
     */
    Obstacle(double startX, double startY, double moveSpeed);
    
    /**
     * Constructor: Create obstacle with specific type
     * 
     * @param startX Initial X position
     * @param startY Initial Y position  
     * @param moveSpeed Horizontal movement speed
     * @param type Specific obstacle type to create
     */
    Obstacle(double startX, double startY, double moveSpeed, ObstacleType type);
    
    /**
     * Destructor: Clean up resources
     */
    ~Obstacle();
    
    // ===== Core Action Methods =====
    
    /**
     * Update obstacle position and sprite
     * 
     * @param deltaTime Time elapsed since last frame
     */
    void update(double deltaTime);
    
    /**
     * Render the obstacle sprite to the window
     * 
     * @param window SFML render window
     */
    void render(sf::RenderWindow& window);
    
    // ===== Information Methods (const) =====
    
    /**
     * Get the collision bounding box
     * Returns invisible rectangle that matches sprite bounds
     * 
     * @return Reference to bounding box rectangle
     */
    const sf::RectangleShape& getShape() const;
    
    /**
     * Get the current sprite (for advanced rendering)
     * 
     * @return Reference to current sprite
     */
    const sf::Sprite& getSprite() const;
    
    /**
     * Get current X position
     * 
     * @return X coordinate
     */
    double getPosX() const;
    
    /**
     * Get current Y position
     * 
     * @return Y coordinate
     */
    double getPosY() const;
    
    /**
     * Get obstacle type
     * 
     * @return Current obstacle type
     */
    ObstacleType getObstacleType() const;
    
    /**
     * Get obstacle size
     * 
     * @return Size vector (width, height)
     */
    sf::Vector2f getSize() const;
    
    /**
     * Check if obstacle has moved off the left side of screen
     * Used by ObstacleManager for cleanup
     * 
     * @return true if obstacle is off-screen
     */
    bool isOffScreen() const;
    
    // ===== Manipulation Methods =====
    
    /**
     * Set the horizontal movement speed
     * Used by ObstacleManager for difficulty scaling
     * 
     * @param newSpeed New horizontal speed (pixels per second)
     */
    void setSpeed(double newSpeed);
    
    /**
     * Change the obstacle type (advanced feature)
     * Switches sprite and adjusts size accordingly
     * 
     * @param newType New obstacle type
     */
    void changeType(ObstacleType newType);

private:
    // ===== Private Helper Methods =====
    
    /**
     * Initialize sprite system based on obstacle type
     * Sets up sprite, size, and collision box
     */
    void initializeSprite();
    
    /**
     * Apply the appropriate sprite for current obstacle type
     * Handles texture loading, scaling, and positioning
     */
    void applySpriteForType();
    
    /**
     * Update the invisible bounding box for collision detection
     * Ensures collision box matches sprite position and size
     */
    void updateBoundingBox();
    
    /**
     * Get sprite type enum from obstacle type
     * Maps obstacle types to texture manager sprite types
     * 
     * @param type Obstacle type
     * @return Corresponding sprite type for TextureManager
     */
    TextureManager::SpriteType getSpriteTypeFromObstacleType(ObstacleType type) const;
    
    /**
     * Get size for specific obstacle type
     * Returns appropriate dimensions for each obstacle type
     * 
     * @param type Obstacle type
     * @return Size vector for the type
     */
    sf::Vector2f getSizeForType(ObstacleType type) const;
    
    /**
     * Generate random obstacle type
     * Used when constructor doesn't specify type
     * Can be weighted for gameplay balance
     * 
     * @return Random obstacle type
     */
    static ObstacleType generateRandomType();

    /**
     * Get collision size for specific obstacle type
     * 
     * @param type Obstacle type
     * @return Collision size vector for the type
     *  */
    sf::Vector2f getCollisionSizeForType(ObstacleType type) const;
};

#endif // OBSTACLE_HPP