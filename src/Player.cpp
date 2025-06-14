#include "Player.hpp"
#include <iostream>

const double Player::GROUND_Y = 400.0; // Ground level Y coordinate
const double Player::JUMP_STRENGTH = -400.0; // Initial jump velocity
const double Player::GRAVITY = 700.0; // Gravity acceleration
const double Player::FAST_FALL_MULTIPLIER = 2.5;        // Enhanced gravity for fast falling
const double Player::FAST_FALL_TERMINAL_VELOCITY = 800.0; // Maximum fall speed to maintain control
const double Player::RUNNING_ANIMATION_SPEED = 8.0;
const sf::Vector2f Player::DEFAULT_SIZE = sf::Vector2f(60.0, 64.5);  // Original Rectangular size

// ===== Triple Collision Box Constants =====
const double Player::HEAD_WIDTH_RATIO = 0.5;    // Head box: narrow and precise
const double Player::HEAD_HEIGHT_RATIO = 0.47;   // Head box: upper 30% of sprite
const double Player::BODY_WIDTH_RATIO = 0.4;    // Body box: main collision area
const double Player::BODY_HEIGHT_RATIO = 0.66;   // Body box: middle 50% of sprite
const double Player::TAIL_WIDTH_RATIO = 0.25;    // Tail box: rear area for ducking
const double Player::TAIL_HEIGHT_RATIO = 0.37;   // Tail box: lower 30% of sprite

Player::Player(double startX, double startY) 
    : posX(startX), 
      posY(startY), 
      velocityY(0.0), 
      isJumping(false),
      isDucking(false),
      isFastFalling(false),        // NEW: Initialize fast fall state
      duckPressed(false),          // NEW: Track duck key state
      animationTimer(0.0f),
      isRunningAnimationFrame1(true),
      targetSize(DEFAULT_SIZE),
      currentSpriteType(TextureManager::SpriteType::DINO_RUNNING_1),
      debugMode(false) { 
    
    // Initialize sprite system
    initializeSprite();
    
    // Set up bounding box for collision detection
    initializeTripleCollisionBoxes();
    // boundingBox.setSize(targetSize);
    // boundingBox.setFillColor(sf::Color::Transparent);  // Invisible
    // boundingBox.setOutlineColor(sf::Color::Red);       // Debug outline (can be removed)
    // boundingBox.setOutlineThickness(3.0f);             // Debug outline (can be removed)
    
    // Update initial positions
    updateTripleCollisionBoxes();
    
    std::cout << "Player initialized with enhanced ducking system at (" << startX << ", " << startY << ")" << std::endl;
}

Player::~Player() {
    // Sprite and bounding box cleanup is automatic
    std::cout << "Player destroyed" << std::endl;
} 

void Player::jump() {
    if (!isJumping && !isDucking) {
        velocityY = JUMP_STRENGTH; // Set initial jump velocity
        isJumping = true; // Set jumping status

        // Change to jumping sprite
        applySpriteType(TextureManager::SpriteType::DINO_JUMPING);
        if (isDucking) {
            applySpriteType(TextureManager::SpriteType::DINO_DUCKING_1);
        }
        
        std::cout << "Player jumped!" << std::endl;
    }
}

void Player::startDucking() {
    duckPressed = true;  // Mark that duck key is being pressed
    targetSize = sf::Vector2f(110.0f * 3 / 4, 53.0f * 3 / 4);
    boundingBox.setSize(targetSize);
    // updateBoundingBox();
    updateTripleCollisionBoxes();
    
    if (isJumping) {
        // ===== AIRBORNE DUCKING: Fast Fall System =====
        /*
         * When player presses duck while jumping, we implement a "fast fall" mechanic.
         * This is common in platformer games to give players more control over their jump.
         * 
         * Key concepts:
         * 1. Jump suppression: If still rising, reduce upward velocity
         * 2. Enhanced gravity: Apply stronger downward force
         * 3. Speed limiting: Prevent uncontrollable fast falling
         */
        if (!isFastFalling) {
            isFastFalling = true;
            
            // Apply immediate downward velocity adjustment
            if (velocityY < 0) {
                // Still rising: cut jump short by reducing upward velocity
                velocityY = 0.3;  // Immediately going down
                std::cout << "Jump cut short - starting fast fall!" << std::endl;
            } else {
                // Already falling: ensure minimum fall speed for responsiveness
                velocityY = std::max(velocityY, 200.0);  // Minimum fall speed
                std::cout << "Fast fall activated!" << std::endl;
            }
            // Change to ducking sprite even in air for visual feedback
            applySpriteType(TextureManager::SpriteType::DINO_DUCKING_1);
            
            
            // Adjust collision box for air ducking
            targetSize = sf::Vector2f(110.0f * 3 / 4, 53.0f * 3 / 4);
            boundingBox.setSize(targetSize);
            // updateBoundingBox();
            updateTripleCollisionBoxes();
        }
    } else {
        // ===== GROUND DUCKING: Traditional Crouch =====
        /*
         * Traditional ducking when on ground - player crouches to avoid low obstacles.
         * This changes both visual appearance and collision box to make player shorter.
         */
        if (!isDucking) {
            isDucking = true;
            
            // Change to ducking sprite
            if (isRunningAnimationFrame1) {
                applySpriteType(TextureManager::SpriteType::DINO_DUCKING_1);
            } else {
                applySpriteType(TextureManager::SpriteType::DINO_DUCKING_2);
            }
            
            // Adjust size for ducking (wider, shorter based on sprite dimensions)
            targetSize = sf::Vector2f(110.0f * 3 / 4, 53.0f * 3 / 4);
            boundingBox.setSize(targetSize);
            
            // Adjust Y position to keep player on ground level
            // This calculation ensures the bottom of the ducking sprite aligns with ground
            posY = GROUND_Y - targetSize.y + DEFAULT_SIZE.y;
            
            // updateBoundingBox();
            updateTripleCollisionBoxes();
            
            std::cout << "Ground ducking activated!" << std::endl;
        }
    }
}

void Player::stopDucking() {
    duckPressed = false;  // Mark that duck key is released
    
    if (isJumping) {
        // ===== AIRBORNE: Stop Fast Fall =====
        /*
         * When player releases duck key while still in air, we stop the fast fall
         * and return to normal jumping physics and appearance.
         */
        if (isFastFalling) {
            isFastFalling = false;
            // Restore normal collision size
            targetSize = DEFAULT_SIZE;
            boundingBox.setSize(targetSize);
            
            // Return to normal jumping sprite
            applySpriteType(TextureManager::SpriteType::DINO_RUNNING_1);
            
            // updateBoundingBox();
            updateTripleCollisionBoxes();
            
            std::cout << "Fast fall deactivated - normal fall resumed!" << std::endl;
        }
    } else {
        // ===== GROUND: Stop Traditional Duck =====
        /*
         * When player releases duck key while on ground, return to normal standing/running state.
         */
        if (isDucking) {
            isDucking = false;
            
            // Return to normal size and position
            targetSize = DEFAULT_SIZE;
            boundingBox.setSize(targetSize);
            posY = GROUND_Y;  // Reset to normal ground level
            
            // Return to running animation
            updateSprite();
            // updateBoundingBox();
            updateTripleCollisionBoxes();
            
            std::cout << "Ground ducking deactivated!" << std::endl;
        }
    }
}

void Player::update(double deltaTime) {
    // ===== Enhanced Physics Simulation =====
    /*
     * The physics update now handles multiple gravity states:
     * 1. Normal gravity when just jumping
     * 2. Enhanced gravity when fast falling
     * 3. Speed limiting to maintain game balance
     */
    if (isJumping) {
        // Calculate gravity based on current ducking state
        double currentGravity = GRAVITY;
        
        if (isFastFalling) {
            // Apply enhanced gravity for fast falling
            currentGravity *= FAST_FALL_MULTIPLIER;
            
            // Cap the maximum fall speed for game balance and control
            if (velocityY > FAST_FALL_TERMINAL_VELOCITY) {
                velocityY = FAST_FALL_TERMINAL_VELOCITY;
            }
        }
        
        // Apply gravity (normal or enhanced) and update position
        velocityY += currentGravity * deltaTime;
        posY += velocityY * deltaTime;
        
        // ===== Landing Logic with State Management =====
        /*
         * When player lands, we need to determine the correct ground state
         * based on whether the duck key is still being held.
         */
        if (posY >= GROUND_Y) {
            // Player has landed - reset physics
            posY = GROUND_Y;
            velocityY = 0.0;
            isJumping = false;
            isFastFalling = false;  // Always reset fast fall on landing
            
            std::cout << "Player landed!" << std::endl;
            
            // Determine ground state based on duck key
            if (duckPressed) {
                // Duck key still held - transition to ground ducking
                isDucking = true;
                if (isRunningAnimationFrame1) {
                    applySpriteType(TextureManager::SpriteType::DINO_DUCKING_1);
                } else {
                    applySpriteType(TextureManager::SpriteType::DINO_DUCKING_2);
                }
                
                // Adjust for ground ducking position and size
                targetSize = sf::Vector2f(110.0f * 3 / 4, 53.0f * 3 / 4);
                boundingBox.setSize(targetSize);
                posY = GROUND_Y - targetSize.y + DEFAULT_SIZE.y;
                
                std::cout << "Landed into ground duck!" << std::endl;
            } else {
                // Duck key not held - return to normal running
                isDucking = false;
                targetSize = DEFAULT_SIZE;
                boundingBox.setSize(targetSize);
                updateSprite();  // This will resume running animation
                
                std::cout << "Landed into normal running!" << std::endl;
            }
        }
    }
    
    // ===== Animation Update =====
    /*
     * Only update running animation when player is on ground and not ducking.
     * This prevents animation conflicts during jumping or ducking states.
     */
    if (!isJumping /*&& !isDucking*/) {
        updateRunningAnimation(deltaTime);
    }
    
    // ===== Position Synchronization =====
    /*
     * Ensure both sprite and collision box are always synchronized.
     * This is crucial for maintaining accurate collision detection.
     */
    currentSprite.setPosition(posX, posY);
    // updateBoundingBox();
    updateTripleCollisionBoxes();
}

void Player::render(sf::RenderWindow& window) {
    // FIXED: Render the sprite instead of the old shape
    window.draw(currentSprite);  // Draw the actual sprite
    
    //===== Optionally render debug bounding box (comment out for release) ==========
    // only render if debug mode is enabled
    if (debugMode) {
        window.draw(headCollisionBox);  // Blue head box
        window.draw(bodyCollisionBox);  // Red body box
        window.draw(tailCollisionBox);  // Green tail box
        // window.draw(boundingBox);    // Yellow legacy box (optional)
    }
}

void Player::reset() {
    // Reset all physics and state variables
    posX = 100.0;
    posY = GROUND_Y;
    velocityY = 0.0;
    isJumping = false;
    isDucking = false;
    isFastFalling = false;    // NEW: Reset fast fall state
    duckPressed = false;      // NEW: Reset duck key state
    
    // Reset animation state
    animationTimer = 0.0f;
    isRunningAnimationFrame1 = true;
    
    // Reset size and collision box
    targetSize = DEFAULT_SIZE;
    boundingBox.setSize(targetSize);
    
    // Reset sprite to initial running frame
    applySpriteType(TextureManager::SpriteType::DINO_RUNNING_1);
    
    // Update positions
    currentSprite.setPosition(posX, posY);
    // updateBoundingBox();
    updateTripleCollisionBoxes();
    
    std::cout << "Player reset to initial state with enhanced ducking system" << std::endl;
}

// ===== Information Methods =====

const sf::RectangleShape& Player::getHeadCollisionBox() const {
    return headCollisionBox;
}

const sf::RectangleShape& Player::getBodyCollisionBox() const {
    return bodyCollisionBox;
}

const sf::RectangleShape& Player::getTailCollisionBox() const {
    return tailCollisionBox;
}

std::vector<sf::RectangleShape> Player::getAllCollisionBoxes() const {
    return {headCollisionBox, bodyCollisionBox, tailCollisionBox};
}


const sf::RectangleShape& Player::getShape() const {
    return boundingBox;
}

const sf::Sprite& Player::getSprite() const {
    return currentSprite;
}

bool Player::getIsJumping() const {
    return isJumping;
}

bool Player::getIsDucking() const {
    return isDucking || isFastFalling;  // Consider fast fall as a form of ducking
}

bool Player::getIsFastFalling() const {
    return isFastFalling;
}

double Player::getPosX() const {
    return posX;
}

double Player::getPosY() const {
    return posY;
}

sf::Vector2f Player::getSize() const {
    return targetSize;
}

void Player::setDebugMode(bool enabled) {
    debugMode = enabled;
    std::cout << "Debug mode " << (enabled ? "enabled" : "disabled") << std::endl;
}

bool Player::isDebugMode() const {
    return debugMode;
}

void Player::toggleDebugMode() {
    debugMode = !debugMode;
    std::cout << "Debug mode toggled " << (debugMode ? "ON" : "OFF") << std::endl;
}

// ===== Private Helper Methods =====

void Player::updateSprite() {
    /*
     * State-based sprite selection logic.
     * Priority: Jumping > Ducking > Running Animation
     */
    if (isJumping) {
        applySpriteType(TextureManager::SpriteType::DINO_JUMPING);
        if (isDucking) {
            applySpriteType(TextureManager::SpriteType::DINO_DUCKING_1);
        }
    } else if (isDucking || isFastFalling) {
        if (isRunningAnimationFrame1) {
            applySpriteType(TextureManager::SpriteType::DINO_DUCKING_1);
        } else {
            applySpriteType(TextureManager::SpriteType::DINO_DUCKING_2);
        }
    } else {
        // Use current running animation frame
        if (isRunningAnimationFrame1) {
            applySpriteType(TextureManager::SpriteType::DINO_RUNNING_1);
        } else {
            applySpriteType(TextureManager::SpriteType::DINO_RUNNING_2);
        }
    }
}

void Player::updateRunningAnimation(float deltaTime) {
    /*
     * Frame-based animation system for smooth running motion.
     * Uses delta time to ensure consistent animation speed regardless of framerate.
     */
    animationTimer += deltaTime;
    
    // Check if it's time to switch animation frames
    if (animationTimer >= (1.0f / RUNNING_ANIMATION_SPEED)) {
        isRunningAnimationFrame1 = !isRunningAnimationFrame1;
        animationTimer = 0.0f;
        
        // Apply the new animation frame
        updateSprite();
    }
}

void Player::applySpriteType(TextureManager::SpriteType spriteType) {
    /*
     * Central sprite application method that handles texture loading,
     * scaling, and positioning through the TextureManager system.
     */
    currentSpriteType = spriteType;
    
    // Get texture manager instance and create properly sized sprite
    TextureManager& textureManager = TextureManager::getInstance();
    currentSprite = textureManager.createSprite(spriteType, targetSize);
    
    // Position the sprite at current player position
    currentSprite.setPosition(posX, posY);
}

void Player::updateBoundingBox() {
    /*
     * Keep collision bounding box synchronized with sprite position.
     * This ensures accurate collision detection regardless of sprite changes.
     */
    boundingBox.setPosition(posX, posY);
}

void Player::initializeSprite() {
    /*
     * Initialize the sprite system with the first running animation frame.
     * This sets up the player's visual representation at game start.
     */
    applySpriteType(TextureManager::SpriteType::DINO_RUNNING_1);
    
    std::cout << "Player sprite system initialized with enhanced ducking support" << std::endl;
}

// ===== Triple Collision Box Management =====

void Player::initializeTripleCollisionBoxes() {
    // Head collision box setup (Blue outline for debugging)
    headCollisionBox.setFillColor(sf::Color::Transparent);
    headCollisionBox.setOutlineColor(sf::Color::Blue);
    headCollisionBox.setOutlineThickness(2.0f);
    
    // Body collision box setup (Red outline for debugging)
    bodyCollisionBox.setFillColor(sf::Color::Transparent);
    bodyCollisionBox.setOutlineColor(sf::Color::Red);
    bodyCollisionBox.setOutlineThickness(2.0f);
    
    // Tail collision box setup (Green outline for debugging)
    tailCollisionBox.setFillColor(sf::Color::Transparent);
    tailCollisionBox.setOutlineColor(sf::Color::Green);
    tailCollisionBox.setOutlineThickness(2.0f);
    
    // Legacy bounding box setup (Yellow outline for debugging)
    boundingBox.setFillColor(sf::Color::Transparent);
    boundingBox.setOutlineColor(sf::Color::Yellow);
    boundingBox.setOutlineThickness(2.0f);
    
    std::cout << "Triple collision boxes initialized" << std::endl;
}

void Player::updateTripleCollisionBoxes() {
    if (isDucking) {
        updateDuckingStateCollision();
    } else if (isFastFalling) {
        // If fast falling, adjust body collision box to be lower
        double headOffsetY = targetSize.y * 0.1; // Lower the head box slightly
        headCollisionBox.setPosition(headCollisionBox.getPosition().x, posY + headOffsetY);
        double bodyOffsetY = targetSize.y * 0.1; // Lower the body box slightly
        bodyCollisionBox.setPosition(bodyCollisionBox.getPosition().x, posY + bodyOffsetY);
        double tailOffsetY = targetSize.y * 0.02; // Lower the tail box slightly
        double tailOffsetX = targetSize.x * 0.07; // Keep tail box at the back
        tailCollisionBox.setSize(sf::Vector2f(DEFAULT_SIZE.x * TAIL_WIDTH_RATIO, tailCollisionBox.getSize().y));
        tailCollisionBox.setPosition(posX + tailOffsetX, posY + tailOffsetY);
    } else if (isJumping) {
        updateJumpingStateCollision();
    } else {
        updateNormalStateCollision();
    }
    
    // Update legacy bounding box for backward compatibility
    updateLegacyBoundingBox();
}

void Player::updateNormalStateCollision() {
    // Head collision box (upper area, narrow)
    double headWidth = targetSize.x * HEAD_WIDTH_RATIO;
    double headHeight = targetSize.y * HEAD_HEIGHT_RATIO;
    double headOffsetX = targetSize.x - headWidth;
    double headOffsetY = 0.0;
    
    headCollisionBox.setSize(sf::Vector2f(headWidth, headHeight));
    headCollisionBox.setPosition(posX + headOffsetX, posY + headOffsetY);
    
    // Body collision box (middle area, main collision)
    double bodyWidth = targetSize.x * BODY_WIDTH_RATIO;
    double bodyHeight = targetSize.y * BODY_HEIGHT_RATIO;
    double bodyOffsetX = (targetSize.x - bodyWidth / 0.55);
    double bodyOffsetY = targetSize.y * HEAD_HEIGHT_RATIO / 1.4;
    
    bodyCollisionBox.setSize(sf::Vector2f(bodyWidth, bodyHeight));
    bodyCollisionBox.setPosition(posX + bodyOffsetX, posY + bodyOffsetY);
    
    // Tail collision box (lower area, rear detection)
    double tailWidth = targetSize.x * TAIL_WIDTH_RATIO;
    double tailHeight = targetSize.y * TAIL_HEIGHT_RATIO;
    double tailOffsetX = 0.0;  // Position towards the back
    double tailOffsetY = targetSize.y * 0.3;  // Lower position
    
    tailCollisionBox.setSize(sf::Vector2f(tailWidth, tailHeight));
    tailCollisionBox.setPosition(posX - tailOffsetX, posY + tailOffsetY);
}

void Player::updateDuckingStateCollision() {
    // Head collision box (smaller when ducking)
    double headWidth = targetSize.x * (HEAD_WIDTH_RATIO * 0.8f);
    double headHeight = targetSize.y * (HEAD_HEIGHT_RATIO * 0.9f);
    double headOffsetX = (targetSize.x - headWidth);
    double headOffsetY = targetSize.y * 0.1f;
    
    headCollisionBox.setSize(sf::Vector2f(headWidth, headHeight));
    headCollisionBox.setPosition(posX + headOffsetX, posY + headOffsetY);
    
    // Body collision box (wider and lower when ducking)
    double bodyWidth = targetSize.x * (BODY_WIDTH_RATIO * 1.3f);
    double bodyHeight = targetSize.y * (BODY_HEIGHT_RATIO * 0.8f);
    double bodyOffsetX = (targetSize.x - bodyWidth) / 2.0f;
    double bodyOffsetY = targetSize.y * 0.3f;
    
    bodyCollisionBox.setSize(sf::Vector2f(bodyWidth, bodyHeight));
    bodyCollisionBox.setPosition(posX + bodyOffsetX, posY + bodyOffsetY);
    
    // Tail collision box (more prominent when ducking)
    double tailWidth = targetSize.x * (TAIL_WIDTH_RATIO * 1.2f);
    double tailHeight = targetSize.y * (TAIL_HEIGHT_RATIO * 0.9f);
    double tailOffsetX = targetSize.x;
    double tailOffsetY = targetSize.y * 0.3f;
    
    tailCollisionBox.setSize(sf::Vector2f(tailWidth, tailHeight));
    tailCollisionBox.setPosition(posX, posY);
}

void Player::updateJumpingStateCollision() {
    // Use normal state collision during jumping
    updateNormalStateCollision();
}

void Player::updateLegacyBoundingBox() {
    // Legacy bounding box follows body collision box for compatibility
    boundingBox.setSize(bodyCollisionBox.getSize());
    boundingBox.setPosition(bodyCollisionBox.getPosition());
}