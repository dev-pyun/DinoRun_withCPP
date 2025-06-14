#ifndef PLAYER_HPP
#define PLAYER_HPP

// #define DEBUG_COLLISION  // Enable debug collision boxes for development

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
    sf::RectangleShape headCollisionBox; // For head collision detection (invisible)
    sf::RectangleShape bodyCollisionBox; // For bottom collision detection (invisible)
    sf::RectangleShape tailCollisionBox; // For ducking collision detection (invisible)
    sf::RectangleShape boundingBox; // Invisible rectangle for collision detection

    // ===== Triple Collision Box Constants =====
    static const double HEAD_WIDTH_RATIO;     // Head box width ratio (0.5)
    static const double HEAD_HEIGHT_RATIO;    // Head box height ratio (0.3)
    static const double BODY_WIDTH_RATIO;     // Body box width ratio (0.7)
    static const double BODY_HEIGHT_RATIO;    // Body box height ratio (0.5)
    static const double TAIL_WIDTH_RATIO;     // Tail box width ratio (0.4)
    static const double TAIL_HEIGHT_RATIO;    // Tail box height ratio (0.3)

    // ===== Physics Constants =====
    static const double GROUND_Y;
    static const double JUMP_STRENGTH;
    static const double GRAVITY;
    static const double FAST_FALL_MULTIPLIER;        // Enhanced gravity multiplier for fast falling
    static const double FAST_FALL_TERMINAL_VELOCITY; // Maximum fall speed when fast falling

    // ===== Animation Constants =====
    static const double RUNNING_ANIMATION_SPEED;  // Frames per second for running
    static const sf::Vector2f DEFAULT_SIZE;      // Default player size (matches original rectangle)

    // ===== runtime debugging =====
    bool debugMode;


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

    // ===== Information Methods (const: guaranteed not to modify data) =====

    /**
     * Get the head collision box (upper small area)
     * Used for precise head collision detection
     * 
     * @return Reference to head collision rectangle
     */
    const sf::RectangleShape& getHeadCollisionBox() const;
    
    /**
     * Get the body collision box (main torso area)
     * Used for primary body collision detection
     * 
     * @return Reference to body collision rectangle
     */
    const sf::RectangleShape& getBodyCollisionBox() const;
    
    /**
     * Get the tail collision box (lower back area)
     * Used for ducking collision detection
     * 
     * @return Reference to tail collision rectangle
     */
    const sf::RectangleShape& getTailCollisionBox() const;
    
    /**
     * Get all collision boxes as a vector
     * Useful for collision managers that check multiple areas
     * 
     * @return Vector containing all three collision boxes
     */
    std::vector<sf::RectangleShape> getAllCollisionBoxes() const;
    
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

    // ===== Debugging Methods =====
        /**
     * Toggle debug collision box visibility
     * 
     * @param enabled true to show collision boxes, false to hide
     */
    void setDebugMode(bool enabled);
    
    /**
     * Get current debug mode state
     * 
     * @return true if debug mode is enabled
     */
    bool isDebugMode() const;
    
    /**
     * Toggle debug mode on/off
     */
    void toggleDebugMode();

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

    // ===== Triple Collision Box Management =====
    
    /**
     * Initialize all three collision boxes with proper visual debugging properties
     * Sets up colors, transparency, and outline for debugging visualization
     */
    void initializeTripleCollisionBoxes();
    
    /**
     * Update all collision boxes based on current player state
     * Handles position, size, and state-specific adjustments
     */
    void updateTripleCollisionBoxes();
    
    /**
     * Update collision boxes for normal standing/running state
     * Standard three-box configuration for regular gameplay
     */
    void updateNormalStateCollision();
    
    /**
     * Update collision boxes for ducking state
     * Adjusts boxes for lowered profile when ducking
     */
    void updateDuckingStateCollision();
    
    /**
     * Update collision boxes for jumping state
     * Maintains boxes during aerial movement
     */
    void updateJumpingStateCollision();
    
    /**
     * Update legacy bounding box for backward compatibility
     * Synchronizes legacy box with body collision box
     */
    void updateLegacyBoundingBox();

};


#endif // PLAYER_HPP