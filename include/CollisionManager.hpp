#ifndef COLLISION_MANAGER_HPP
#define COLLISION_MANAGER_HPP

#include <SFML/Graphics.hpp>
#include <vector>

// Forward declarations for our game classes
class Player;
class Obstacle;
class ObstacleManager;

/**
 * CollisionManager class: Static utility class for all collision detection in the game
 * 
 * Design Philosophy:
 * - Pure utility class with no state (all static methods)
 * - Centralized collision logic for consistency and maintainability
 * - Extensible design to support various collision types in the future
 * - Performance-optimized algorithms for real-time game requirements
 * 
 * This class encapsulates all collision detection algorithms and provides
 * a clean interface for different types of collision checks that may occur
 * in the game. By centralizing this logic, we ensure consistency across
 * all collision detection and make it easy to optimize or extend.
 */
class CollisionManager {
public:
    // ===== Collision Type Enumeration =====
    
    /**
     * Enumeration for different types of collision detection results
     * Provides detailed information about which collision box was hit
     */
    enum class CollisionType {
        NO_COLLISION,       // No collision detected
        HEAD_COLLISION,     // Only head collision box hit
        BODY_COLLISION,     // Only body collision box hit  
        TAIL_COLLISION,     // Only tail collision box hit
        MULTIPLE_COLLISION, // Multiple collision boxes hit
        LEGACY_COLLISION    // Legacy single-box collision (for compatibility)
    };
    
    /**
     * Structure to hold comprehensive collision information
     * Enhanced to support triple collision box system
     */
    struct CollisionInfo {
        bool hasCollision;              // Whether any collision occurred
        CollisionType collisionType;    // Type of collision detected
        sf::Vector2f collisionPoint;    // Point where collision happened
        sf::Vector2f normal;            // Normal vector at collision point
        double penetrationDepth;        // How deep objects are overlapping
        
        // Triple collision box specific information
        bool headHit;                   // Whether head box was hit
        bool bodyHit;                   // Whether body box was hit
        bool tailHit;                   // Whether tail box was hit
        
        CollisionInfo() : hasCollision(false), collisionType(CollisionType::NO_COLLISION),
                         collisionPoint(0, 0), normal(0, 0), penetrationDepth(0),
                         headHit(false), bodyHit(false), tailHit(false) {}
    };

    // ===== Basic Collision Detection Methods =====
    
    /**
     * Check collision between two SFML rectangles using AABB (Axis-Aligned Bounding Box)
     * This is the most basic form of collision detection, suitable for rectangular objects
     * 
     * @param rect1 First rectangle to check
     * @param rect2 Second rectangle to check
     * @return true if rectangles overlap, false otherwise
     */
    static bool checkRectangleCollision(const sf::RectangleShape& rect1, const sf::RectangleShape& rect2);
    
    /**
     * Check collision between multiple rectangles and a single rectangle
     * Used for triple collision box system
     * 
     * @param multipleBoxes Vector of rectangles to check
     * @param singleBox Single rectangle to check against
     * @return true if any rectangle collides, false otherwise
     */
    static bool checkMultipleRectangleCollision(const std::vector<sf::RectangleShape>& multipleBoxes, 
        const sf::RectangleShape& singleBox);

    // ===== Enhanced Game-Specific Collision Methods =====
    
    /**
     * Check if player collides with any obstacle using triple collision box system
     * This is the new enhanced collision check used during gameplay
     * 
     * @param player The player object to check (with triple collision boxes)
     * @param obstacleManager The obstacle manager containing all obstacles
     * @return true if player collides with any obstacle, false otherwise
     */
    static bool checkPlayerObstacleCollisionTriple(const Player& player, const ObstacleManager& obstacleManager);
    
    /**
     * Check collision between player and a single obstacle using triple collision system
     * Provides detailed collision information about which boxes were hit
     * 
     * @param player The player object to check
     * @param obstacle The specific obstacle to check against
     * @return CollisionInfo with detailed collision data
     */
    static CollisionInfo checkPlayerSingleObstacleTriple(const Player& player, const Obstacle& obstacle);
    
    /**
     * Get collision type between player and obstacle
     * Determines which specific collision boxes were hit
     * 
     * @param player The player object
     * @param obstacle The obstacle object
     * @return CollisionType indicating the type of collision
     */
    static CollisionType getPlayerObstacleCollisionType(const Player& player, const Obstacle& obstacle);
    
    // ===== Legacy Collision Methods =====
    
    /**
     * Check if player collides with any obstacle in the obstacle manager
     * This is the main collision check used during gameplay
     * 
     * @param player The player object to check
     * @param obstacleManager The obstacle manager containing all obstacles
     * @return true if player collides with any obstacle, false otherwise
     */
    static bool checkPlayerObstacleCollision(const Player& player, const ObstacleManager& obstacleManager);
    
    /**
     * Check collision between player and a single obstacle
     * Useful for more granular collision detection or special effects
     * 
     * @param player The player object to check
     * @param obstacle The specific obstacle to check against
     * @return true if collision detected, false otherwise
     */
    static bool checkPlayerSingleObstacle(const Player& player, const Obstacle& obstacle);
    
    // ===== Advanced Collision Information =====
    /**
     * Get detailed collision information between two rectangles
     * Provides collision point, normal, and penetration depth for advanced physics
     * 
     * @param rect1 First rectangle
     * @param rect2 Second rectangle
     * @return CollisionInfo structure with detailed collision data
     */
    static CollisionInfo getDetailedCollision(const sf::RectangleShape& rect1, const sf::RectangleShape& rect2);
    
    // ===== Utility Methods for Triple Collision System =====
    
    /**
     * Count how many collision boxes are hit
     * Useful for determining collision severity
     * 
     * @param player The player object
     * @param obstacle The obstacle object
     * @return Number of collision boxes hit (0-3)
     */
    static int countCollisionBoxesHit(const Player& player, const Obstacle& obstacle);
    
    /**
     * Check if specific collision box type is hit
     * Allows checking for specific collision scenarios
     * 
     * @param player The player object
     * @param obstacle The obstacle object
     * @param boxType Which box to check ("head", "body", or "tail")
     * @return true if specified box is hit
     */
    static bool isSpecificBoxHit(const Player& player, const Obstacle& obstacle, const std::string& boxType);

    // ===== Future Expansion Methods =====
    
    /**
     * Check collision between circle and rectangle
     * Planned for future circular items or obstacles
     * 
     * @param circle Circle object (position and radius)
     * @param rectangle Rectangle object
     * @return true if collision detected, false otherwise
     */
    static bool checkCircleRectangleCollision(const sf::Vector2f& circleCenter, double radius, 
                                            const sf::RectangleShape& rectangle);
    
    /**
     * Check collision between two circles
     * Planned for future circular game elements
     * 
     * @param center1 Center of first circle
     * @param radius1 Radius of first circle
     * @param center2 Center of second circle
     * @param radius2 Radius of second circle
     * @return true if circles overlap, false otherwise
     */
    static bool checkCircleCircleCollision(const sf::Vector2f& center1, double radius1,
                                         const sf::Vector2f& center2, double radius2);
    
private:
    // ===== Private Helper Methods =====
    
    /**
     * Calculate distance between two points
     * Helper method for various collision calculations
     * 
     * @param point1 First point
     * @param point2 Second point
     * @return Distance between points
     */
    static double calculateDistance(const sf::Vector2f& point1, const sf::Vector2f& point2);
    
    /**
     * Check if a point is inside a rectangle
     * Helper method for point-in-rectangle tests
     * 
     * @param point Point to test
     * @param rectangle Rectangle to test against
     * @return true if point is inside rectangle
     */
    static bool isPointInsideRectangle(const sf::Vector2f& point, const sf::RectangleShape& rectangle);

    /**
     * Helper method to determine collision type from individual box hits
     * 
     * @param headHit Whether head collision box was hit
     * @param bodyHit Whether body collision box was hit
     * @param tailHit Whether tail collision box was hit
     * @return Appropriate CollisionType
     */
    static CollisionType determineCollisionType(bool headHit, bool bodyHit, bool tailHit);
    
    
    // Private constructor to prevent instantiation (utility class)
    CollisionManager() = delete;
    CollisionManager(const CollisionManager&) = delete;
    CollisionManager& operator=(const CollisionManager&) = delete;
};

#endif // COLLISION_MANAGER_HPP