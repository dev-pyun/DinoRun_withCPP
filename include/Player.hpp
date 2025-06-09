#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <SFML/Graphics.hpp>
#include "TextureManager.hpp"

/**
 * Player class: Enhanced with sprite-based rendering, animation, and advanced ducking system
 * 
 * Major improvements in this version:
 * - Replaced simple rectangle with animated sprites
 * - Added running animation cycle
 * - Support for jumping and ducking sprites
 * - Advanced ducking system with fast fall mechanics
 * - Maintains backward compatibility with collision system
 * - Automatic sprite scaling to match original rectangle size
 * 
 * The ducking system now supports two distinct modes:
 * 1. Ground ducking: Traditional crouch to avoid low obstacles
 * 2. Air ducking (fast fall): Enhanced gravity for quick landing during jumps
 */
class Player {
private:
    sf::RectangleShape shape; // Player shape
    double posX;
    double posY;             // Player position
    double velocityY;         // Vertical velocity for jumping
    bool isJumping;           // Jumping status
    bool isDucking;            // Ducking status
    bool isFastFalling;      // Air ducking state (fast fall)
    bool duckPressed;        // Track if duck key is currently held

    // ===== Sprite System =====
    sf::Sprite currentSprite;
    TextureManager::SpriteType currentSpriteType;

    // ===== Animation System =====
    double animationTimer;
    bool isRunningAnimationFrame1;  // Toggle between running frames
    
    // ===== Size Management =====
    sf::Vector2f targetSize;        // Size to scale sprites to
    sf::RectangleShape boundingBox; // For collision detection (invisible)

    // ===== Physics Constants =====
    static const double GROUND_Y;
    static const double JUMP_STRENGTH;
    static const double GRAVITY;
    static const double FAST_FALL_MULTIPLIER;        // Enhanced gravity multiplier for fast falling
    static const double FAST_FALL_TERMINAL_VELOCITY; // Maximum fall speed when fast falling

    // ===== Animation Constants =====
    static const double RUNNING_ANIMATION_SPEED;  // Frames per second for running
    static const sf::Vector2f DEFAULT_SIZE;      // Default player size (matches original rectangle)

public:
    /**
     * Constructor: Initialize player with sprite support
     * 
     * @param startX Initial X position
     * @param startY Initial Y position
     */
    Player(double startX, double startY);
    /**
     * Destructor: Clean up resources
     */   
    ~Player();

    // 행동 메서드들
    /**
     * Make the player jump
     * Changes to jumping sprite and applies jump physics
     */
    void jump();     

    /**
     * Start ducking action - handles both ground and air ducking
     * Ground: Traditional crouch for avoiding low obstacles
     * Air: Fast fall with enhanced gravity for quick landing
     */
    void startDucking();

    /**
     * Stop ducking action - returns to appropriate state
     * Handles transition from both ground ducking and fast falling
     */
    void stopDucking();

    /**
     * Update player physics and animation
     * Handles sprite animation, physics simulation, and sprite updates
     * 
     * @param deltaTime Time elapsed since last frame
     */
    void update(double deltaTime);   

    /**
     * Render the player sprite to the window
     * 
     * @param window SFML render window
     */
    void render(sf::RenderWindow& window); 

    /**
     * Reset player to initial state
     * Resets position, physics, and sprite to starting conditions
     */
    void reset();                   

    // 정보 제공 메서드들 (const: 데이터를 변경하지 않음을 보장)

    /**
     * Get the collision bounding box (for collision detection)
     * Returns invisible rectangle that matches sprite bounds
     * 
     * @return Reference to bounding box rectangle
     */
    const sf::RectangleShape& getShape() const;
    
    /**
     * Get the current sprite (for advanced rendering or effects)
     * 
     * @return Reference to current sprite
     */
    const sf::Sprite& getSprite() const;

    
    /**
     * Check if player is currently jumping
     * 
     * @return true if jumping, false if on ground
     */
    bool getIsJumping() const;
    
    /**
     * Check if player is currently ducking
     * 
     * @return true if ducking, false otherwise
     */
    bool getIsDucking() const;

    /**
     * Check if player is currently fast falling (air ducking)
     * 
     * @return true if fast falling, false otherwise
     */
    bool getIsFastFalling() const;

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
     * Get player's current size
     * 
     * @return Size vector (width, height)
     */
    sf::Vector2f getSize() const;

private:
    // ===== Private Helper Methods =====
    
    /**
     * Update the player's sprite based on current state
     * Chooses appropriate sprite (running, jumping, ducking) and applies it
     */
    void updateSprite();
    
    /**
     * Update running animation frame
     * Cycles between running frames for smooth animation
     * 
     * @param deltaTime Time elapsed since last frame
     */
    void updateRunningAnimation(float deltaTime);
    
    /**
     * Apply the current sprite to the sprite object
     * Handles texture loading, scaling, and positioning
     */
    void applySpriteType(TextureManager::SpriteType spriteType);
    
    /**
     * Update the invisible bounding box for collision detection
     * Ensures collision box matches sprite position and size
     */
    void updateBoundingBox();
    
    /**
     * Initialize sprite system
     * Sets up default sprite and animation state
     */
    void initializeSprite();
};


#endif // PLAYER_HPP