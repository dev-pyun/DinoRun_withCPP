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
     * Check collision between two arbitrary SFML drawable objects
     * Uses the global bounds of each object for collision detection
     * 
     * @param obj1 First drawable object
     * @param obj2 Second drawable object
     * @return true if objects overlap, false otherwise
     */
    static bool checkBoundsCollision(const sf::Drawable& obj1, const sf::Drawable& obj2);
    
    // ===== Game-Specific Collision Methods =====
    
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
     * Structure to hold detailed collision information
     * Used when we need more than just true/false collision result
     */
    struct CollisionInfo {
        bool hasCollision;          // Whether collision occurred
        sf::Vector2f collisionPoint; // Point where collision happened
        sf::Vector2f normal;        // Normal vector at collision point
        double penetrationDepth;    // How deep objects are overlapping
        
        CollisionInfo() : hasCollision(false), collisionPoint(0, 0), normal(0, 0), penetrationDepth(0) {}
    };
    
    /**
     * Get detailed collision information between two rectangles
     * Provides collision point, normal, and penetration depth for advanced physics
     * 
     * @param rect1 First rectangle
     * @param rect2 Second rectangle
     * @return CollisionInfo structure with detailed collision data
     */
    static CollisionInfo getDetailedCollision(const sf::RectangleShape& rect1, const sf::RectangleShape& rect2);
    
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
    
    // Private constructor to prevent instantiation (utility class)
    CollisionManager() = delete;
    CollisionManager(const CollisionManager&) = delete;
    CollisionManager& operator=(const CollisionManager&) = delete;
};

#endif // COLLISION_MANAGER_HPP