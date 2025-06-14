#include "CollisionManager.hpp"
#include "Player.hpp"
#include "Obstacle.hpp"
#include "ObstacleManager.hpp"
#include <cmath>  // for sqrt, pow functions
#include <iostream>

// ===== Basic Collision Detection Methods =====

bool CollisionManager::checkRectangleCollision(const sf::RectangleShape& rect1, const sf::RectangleShape& rect2) {
    // Get the global bounds of both rectangles
    sf::FloatRect bounds1 = rect1.getGlobalBounds();
    sf::FloatRect bounds2 = rect2.getGlobalBounds();
    
    // Use SFML's built-in intersection test
    // This method is optimized and handles edge cases properly
    return bounds1.intersects(bounds2);
}

bool CollisionManager::checkMultipleRectangleCollision(const std::vector<sf::RectangleShape>& multipleBoxes, 
                                                      const sf::RectangleShape& singleBox) {
    // Check if any of the multiple boxes collides with the single box
    for (const auto& box : multipleBoxes) {
        if (checkRectangleCollision(box, singleBox)) {
            return true;  // Early exit on first collision
        }
    }
    return false;  // No collision found
}

// ===== Enhanced Game-Specific Collision Methods =====

bool CollisionManager::checkPlayerObstacleCollisionTriple(const Player& player, const ObstacleManager& obstacleManager) {
    // Get all obstacles from the obstacle manager
    const std::vector<Obstacle>& obstacles = obstacleManager.getObstacles();
    
    // Check collision against each obstacle using triple collision system
    for (const auto& obstacle : obstacles) {
        CollisionInfo info = checkPlayerSingleObstacleTriple(player, obstacle);
        if (info.hasCollision) {
            return true;  // Early exit on first collision found
        }
    }
    
    return false;  // No collision detected
}

CollisionManager::CollisionInfo CollisionManager::checkPlayerSingleObstacleTriple(const Player& player, 
                                                                                  const Obstacle& obstacle) {
    CollisionInfo info;
    
    // Get all collision boxes from player
    const sf::RectangleShape& headBox = player.getHeadCollisionBox();
    const sf::RectangleShape& bodyBox = player.getBodyCollisionBox();
    const sf::RectangleShape& tailBox = player.getTailCollisionBox();
    
    // Get obstacle collision box
    const sf::RectangleShape& obstacleBox = obstacle.getShape();
    
    // Check each collision box individually
    info.headHit = checkRectangleCollision(headBox, obstacleBox);
    info.bodyHit = checkRectangleCollision(bodyBox, obstacleBox);
    info.tailHit = checkRectangleCollision(tailBox, obstacleBox);
    
    // Determine if any collision occurred
    info.hasCollision = info.headHit || info.bodyHit || info.tailHit;
    
    if (info.hasCollision) {
        // Determine collision type
        info.collisionType = determineCollisionType(info.headHit, info.bodyHit, info.tailHit);
        
        // Calculate collision details using the primary collision box (body is primary)
        if (info.bodyHit) {
            CollisionInfo detailedInfo = getDetailedCollision(bodyBox, obstacleBox);
            info.collisionPoint = detailedInfo.collisionPoint;
            info.normal = detailedInfo.normal;
            info.penetrationDepth = detailedInfo.penetrationDepth;
        } else if (info.headHit) {
            CollisionInfo detailedInfo = getDetailedCollision(headBox, obstacleBox);
            info.collisionPoint = detailedInfo.collisionPoint;
            info.normal = detailedInfo.normal;
            info.penetrationDepth = detailedInfo.penetrationDepth;
        } else if (info.tailHit) {
            CollisionInfo detailedInfo = getDetailedCollision(tailBox, obstacleBox);
            info.collisionPoint = detailedInfo.collisionPoint;
            info.normal = detailedInfo.normal;
            info.penetrationDepth = detailedInfo.penetrationDepth;
        }
        
        // Debug output for development
        std::cout << "Triple collision detected - Head: " << info.headHit 
                  << ", Body: " << info.bodyHit 
                  << ", Tail: " << info.tailHit << std::endl;
    }
    
    return info;
}

CollisionManager::CollisionType CollisionManager::getPlayerObstacleCollisionType(const Player& player, 
                                                                                 const Obstacle& obstacle) {
    CollisionInfo info = checkPlayerSingleObstacleTriple(player, obstacle);
    return info.collisionType;
}


// ===== Legacy Collision Methods =====

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
    CollisionManager::CollisionInfo info;
    
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

// ===== Utility Methods for Triple Collision System =====

int CollisionManager::countCollisionBoxesHit(const Player& player, const Obstacle& obstacle) {
    CollisionManager::CollisionInfo info = checkPlayerSingleObstacleTriple(player, obstacle);
    
    int count = 0;
    if (info.headHit) count++;
    if (info.bodyHit) count++;
    if (info.tailHit) count++;
    
    return count;
}

bool CollisionManager::isSpecificBoxHit(const Player& player, const Obstacle& obstacle, const std::string& boxType) {
    CollisionInfo info = checkPlayerSingleObstacleTriple(player, obstacle);
    
    if (boxType == "head") return info.headHit;
    if (boxType == "body") return info.bodyHit;
    if (boxType == "tail") return info.tailHit;
    
    return false;  // Invalid box type
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

CollisionManager::CollisionType CollisionManager::determineCollisionType(bool headHit, bool bodyHit, bool tailHit) {
    // Count how many boxes were hit
    int hitCount = (headHit ? 1 : 0) + (bodyHit ? 1 : 0) + (tailHit ? 1 : 0);
    
    if (hitCount == 0) {
        return CollisionType::NO_COLLISION;
    } else if (hitCount > 1) {
        return CollisionType::MULTIPLE_COLLISION;
    } else {
        // Only one box hit - determine which one
        if (headHit) return CollisionType::HEAD_COLLISION;
        if (bodyHit) return CollisionType::BODY_COLLISION;
        if (tailHit) return CollisionType::TAIL_COLLISION;
    }
    
    return CollisionType::NO_COLLISION;  // Should never reach here
}