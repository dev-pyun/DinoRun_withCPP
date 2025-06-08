#include "CollisionManager.hpp"
#include "Player.hpp"
#include "Obstacle.hpp"
#include "ObstacleManager.hpp"
#include <cmath>  // for sqrt, pow functions

// ===== Basic Collision Detection Methods =====

bool CollisionManager::checkRectangleCollision(const sf::RectangleShape& rect1, const sf::RectangleShape& rect2) {
    // Get the global bounds of both rectangles
    sf::FloatRect bounds1 = rect1.getGlobalBounds();
    sf::FloatRect bounds2 = rect2.getGlobalBounds();
    
    // Use SFML's built-in intersection test
    // This method is optimized and handles edge cases properly
    return bounds1.intersects(bounds2);
}

bool CollisionManager::checkBoundsCollision(const sf::Drawable& obj1, const sf::Drawable& obj2) {
    // This method would require RTTI (Run-Time Type Information) to work properly
    // For now, we'll implement a basic version that works with common SFML shapes
    
    // Note: This is a simplified implementation
    // In a real-world scenario, we might use a visitor pattern or type erasure
    // to handle different drawable types more elegantly
    
    // For our current needs, we'll focus on the rectangle collision
    // which covers our main use case (Player and Obstacle collision)
    
    // This method serves as a placeholder for future expansion
    return false; // Placeholder implementation
}

// ===== Game-Specific Collision Methods =====

bool CollisionManager::checkPlayerObstacleCollision(const Player& player, const ObstacleManager& obstacleManager) {
    // Get the player's shape for collision testing
    const sf::RectangleShape& playerShape = player.getShape();
    
    // Get all obstacles from the obstacle manager
    const std::vector<Obstacle>& obstacles = obstacleManager.getObstacles();
    
    // Check collision against each obstacle
    for (const auto& obstacle : obstacles) {
        if (checkPlayerSingleObstacle(player, obstacle)) {
            return true;  // Early exit on first collision found
        }
    }
    
    return false;  // No collision detected
}

bool CollisionManager::checkPlayerSingleObstacle(const Player& player, const Obstacle& obstacle) {
    // Get shapes from both objects
    const sf::RectangleShape& playerShape = player.getShape();
    const sf::RectangleShape& obstacleShape = obstacle.getShape();
    
    // Use our basic rectangle collision method
    return checkRectangleCollision(playerShape, obstacleShape);
}

// ===== Advanced Collision Information =====

CollisionManager::CollisionInfo CollisionManager::getDetailedCollision(const sf::RectangleShape& rect1, 
                                                                       const sf::RectangleShape& rect2) {
    CollisionInfo info;
    
    // Get bounds of both rectangles
    sf::FloatRect bounds1 = rect1.getGlobalBounds();
    sf::FloatRect bounds2 = rect2.getGlobalBounds();
    
    // Check if collision exists
    sf::FloatRect intersection;
    if (!bounds1.intersects(bounds2, intersection)) {
        info.hasCollision = false;
        return info;
    }
    
    info.hasCollision = true;
    
    // Calculate collision point (center of intersection area)
    info.collisionPoint = sf::Vector2f(
        intersection.left + intersection.width / 2.0,
        intersection.top + intersection.height / 2.0
    );
    
    // Calculate penetration depth (minimum distance to separate objects)
    double overlapX = intersection.width;
    double overlapY = intersection.height;
    info.penetrationDepth = std::min(overlapX, overlapY);
    
    // Calculate collision normal (direction to push objects apart)
    sf::Vector2f center1(bounds1.left + bounds1.width / 2.0, bounds1.top + bounds1.height / 2.0);
    sf::Vector2f center2(bounds2.left + bounds2.width / 2.0, bounds2.top + bounds2.height / 2.0);
    sf::Vector2f direction = center1 - center2;
    
    // Normalize the direction vector to get the normal
    double length = calculateDistance(sf::Vector2f(0, 0), direction);
    if (length > 0) {
        info.normal = sf::Vector2f(direction.x / length, direction.y / length);
    }
    
    return info;
}

// ===== Future Expansion Methods =====

bool CollisionManager::checkCircleRectangleCollision(const sf::Vector2f& circleCenter, double radius, 
                                                    const sf::RectangleShape& rectangle) {
    // Get rectangle bounds
    sf::FloatRect bounds = rectangle.getGlobalBounds();
    
    // Find the closest point on the rectangle to the circle center
    double closestX = std::max(bounds.left, std::min(circleCenter.x, bounds.left + bounds.width));
    double closestY = std::max(bounds.top, std::min(circleCenter.y, bounds.top + bounds.height));
    
    // Calculate distance from circle center to closest point
    double distance = calculateDistance(circleCenter, sf::Vector2f(closestX, closestY));
    
    // Collision occurs if distance is less than or equal to radius
    return distance <= radius;
}

bool CollisionManager::checkCircleCircleCollision(const sf::Vector2f& center1, double radius1,
                                                 const sf::Vector2f& center2, double radius2) {
    // Calculate distance between centers
    double distance = calculateDistance(center1, center2);
    
    // Collision occurs if distance is less than sum of radii
    return distance <= (radius1 + radius2);
}

// ===== Private Helper Methods =====

double CollisionManager::calculateDistance(const sf::Vector2f& point1, const sf::Vector2f& point2) {
    // Use Pythagorean theorem to calculate Euclidean distance
    double dx = point2.x - point1.x;
    double dy = point2.y - point1.y;
    return std::sqrt(dx * dx + dy * dy);
}

bool CollisionManager::isPointInsideRectangle(const sf::Vector2f& point, const sf::RectangleShape& rectangle) {
    sf::FloatRect bounds = rectangle.getGlobalBounds();
    
    // Check if point is within rectangle boundaries
    return (point.x >= bounds.left && 
            point.x <= bounds.left + bounds.width &&
            point.y >= bounds.top && 
            point.y <= bounds.top + bounds.height);
}