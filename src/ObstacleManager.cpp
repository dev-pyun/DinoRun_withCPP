#include "ObstacleManager.hpp"
#include <algorithm>  // for std::remove_if
#include <iostream>   // for debug output in getAverageObstacleDistance

// Static constant definitions - core game balance parameters
const double ObstacleManager::INITIAL_SPAWN_INTERVAL = 2.0;     // Start with 2 seconds between obstacles
const double ObstacleManager::MIN_SPAWN_INTERVAL = 0.8;        // Minimum 1 second at maximum difficulty
const double ObstacleManager::INITIAL_OBSTACLE_SPEED = 200.0;  // Starting speed: 200 pixels/second
const double ObstacleManager::MAX_OBSTACLE_SPEED = 400.0;      // Maximum speed: 400 pixels/second
const double ObstacleManager::SPAWN_POSITION_X = 800.0;        // Spawn at right edge of screen
const double ObstacleManager::SPAWN_POSITION_Y = 424.5;        // Spawn at ground level
const double ObstacleManager::SPEED_INCREASE_RATE = 5.0;       // Speed increases by 5 px/s every second
const double ObstacleManager::INTERVAL_DECREASE_RATE = 0.03;   // Interval decreases by 0.01s every second

// ===== Pattern System Constants =====
const double ObstacleManager::PATTERN_COOLDOWN_TIME = 4.0;
const double ObstacleManager::DIFFICULTY_INCREASE_RATE = 0.04;
const int ObstacleManager::MAX_CONSECUTIVE_HARD = 2;

// Constructor: Initialize ObstacleManager with default values
ObstacleManager::ObstacleManager() 
    : spawnTimer(0.0), 
      obstacleInterval(INITIAL_SPAWN_INTERVAL),
      obstacleSpeed(INITIAL_OBSTACLE_SPEED),
      lastPattern(ObstaclePattern::SINGLE_SMALL),
      consecutiveHardPatterns(0),
      randomGenerator(std::random_device{}()),
      uniformDist(0.0, 1.0),
      currentDifficulty(0.0),
      patternCooldownTimer(0.0) {
        initializePatterns();
    
        // Check if patterns were initialized correctly
        if (patternDefinitions.empty()) {
            std::cerr << "ERROR: Pattern definitions not initialized!" << std::endl;
        } else {
            std::cout << "Enhanced ObstacleManager initialized with " 
                      << patternDefinitions.size() << " patterns" << std::endl;
        }
}

// Destructor: Clean up resources (automatic cleanup for std::vector)
ObstacleManager::~ObstacleManager() {
    // std::vector<Obstacle> automatically handles cleanup
    // Each Obstacle object will also have its destructor called automatically
    std::cout << "Enhanced ObstacleManager destroyed" << std::endl;
}

// Main update method: Called every frame to manage all obstacle-related logic
void ObstacleManager::update(double deltaTime, double gameTime) {
    // Step 1: Update difficulty based on elapsed game time
    updateDifficulty(gameTime);
    updateAvailablePatterns(gameTime); // Update available patterns based on game time
    patternCooldownTimer += deltaTime; // Increment cooldown timer for complex patterns
    
    // Step 2: Update positions of all existing obstacles
    updateExistingObstacles(deltaTime);
    
    // Step 3: Check if it's time to spawn a new obstacle
    spawnTimer += deltaTime;
    if (spawnTimer >= obstacleInterval) {
        ObstaclePattern selectedPattern = selectPattern(gameTime);
        spawnPattern(selectedPattern); // Spawn obstacles based on selected pattern
        std::cout << "Spawned pattern: " << static_cast<int>(selectedPattern) << std::endl;

        spawnTimer = generateRandomOffset(-0.1, 0.1);
    }
    
    // Step 4: Remove obstacles that have moved off-screen
    removeOffScreenObstacles();
}

// Render method: Draw all obstacles to the screen
void ObstacleManager::render(sf::RenderWindow& window) {
    // Iterate through all obstacles and call their individual render methods
    for (auto& obstacle : obstacles) {
        obstacle.render(window);
    }
}

// Clear method: Reset manager to initial state (used when restarting game)
void ObstacleManager::clear() {
    obstacles.clear();                          // Remove all obstacles from vector
    spawnTimer = 0.0;                          // Reset spawn timer
    obstacleInterval = INITIAL_SPAWN_INTERVAL; // Reset spawn interval to initial value
    obstacleSpeed = INITIAL_OBSTACLE_SPEED;    // Reset speed to initial value
    currentDifficulty = 0.0;
    consecutiveHardPatterns = 0;
    patternCooldownTimer = 0.0;
    lastPattern = ObstaclePattern::SINGLE_SMALL;
    availablePatterns.clear();
}

// Difficulty update method: Progressively increase challenge based on game time
void ObstacleManager::updateDifficulty(double gameTime) {
    // Calculate new obstacle speed: base speed + (time * increase rate)
    // Example: 0s=200, 20s=300, 40s=400 (capped at maximum)
    obstacleSpeed = INITIAL_OBSTACLE_SPEED + (gameTime * SPEED_INCREASE_RATE);
    if (obstacleSpeed > MAX_OBSTACLE_SPEED) {
        obstacleSpeed = MAX_OBSTACLE_SPEED;  // Cap at maximum speed
    }
    
    // Calculate new spawn interval: base interval - (time * decrease rate)
    // Example: 0s=2.0s, 50s=1.5s, 100s=1.0s (capped at minimum)
    obstacleInterval = INITIAL_SPAWN_INTERVAL - (gameTime * INTERVAL_DECREASE_RATE);
    if (obstacleInterval < MIN_SPAWN_INTERVAL) {
        obstacleInterval = MIN_SPAWN_INTERVAL;  // Cap at minimum interval
    }

    // Update pattern difficulty based on game time
    currentDifficulty = std::min(1.0, gameTime * DIFFICULTY_INCREASE_RATE);
}

double ObstacleManager::getCurrentDifficulty() const {
    return currentDifficulty; // Return current difficulty level (0.0 - 1.0)
}

// ===== Pattern Management =====

ObstacleManager::ObstaclePattern ObstacleManager::selectPattern(double gameTime) {
    // for debugging purposes, print available patterns and current difficulty
    std::cout << "Current game time: " << gameTime << "s" << std::endl;
    std::cout << "Selecting pattern - Available: " << availablePatterns.size() 
              << ", Difficulty: " << currentDifficulty << std::endl;
    
    if (availablePatterns.empty()) {
        std::cout << "No available patterns, using SINGLE_SMALL" << std::endl;
        return ObstaclePattern::SINGLE_SMALL;
    }

    // Use weighted selection based on current difficulty
    ObstaclePattern selectedPattern = weightedPatternSelection(gameTime);

    // Ensure the selected pattern exists in the definitions
    if (patternDefinitions.find(selectedPattern) == patternDefinitions.end()) {
        std::cout << "Selected pattern not found in definitions! Using SINGLE_SMALL" << std::endl;
        selectedPattern = ObstaclePattern::SINGLE_SMALL;
    }
    
    // Check if we should avoid this pattern due to consecutive hard patterns or cooldown
    if (shouldAvoidPattern(selectedPattern)) {
        // Fall back to easier patterns
        if (currentDifficulty < 0.3) {
            selectedPattern = ObstaclePattern::SINGLE_SMALL;
        } else if (currentDifficulty < 0.6) {
            selectedPattern = (uniformDist(randomGenerator) < 0.5) ? 
                             ObstaclePattern::SINGLE_SMALL : ObstaclePattern::SINGLE_MID;
        } else {
            selectedPattern = ObstaclePattern::WIDE_GAP;  // Give player a breather
        }
    }
    
    // Update consecutive hard pattern counter
    if (patternDefinitions[selectedPattern].patternDifficulty > 0.6) {
        consecutiveHardPatterns++;
    } else {
        consecutiveHardPatterns = 0;
    }
    
    lastPattern = selectedPattern;
    return selectedPattern;
}

void ObstacleManager::spawnPattern(ObstaclePattern pattern) {
    auto it = patternDefinitions.find(pattern);
    if (it == patternDefinitions.end()) {
        std::cout << "Warning: Pattern not found, spawning default single small" << std::endl;
        spawnSingleObstacle(Obstacle::ObstacleType::CACTUS_SMALL, 0.0);
        return;
    }
    
    const PatternDefinition& def = it->second;
    
    std::cout << "Spawning pattern: " << def.patternName 
              << " (Difficulty: " << def.patternDifficulty << ")" << std::endl;
    
    // Spawn each obstacle in the pattern
    for (size_t i = 0; i < def.obstacleTypes.size(); ++i) {
        double xOffset = (i < def.relativePositions.size()) ? def.relativePositions[i] : 0.0;
        
        // Add slight randomization to prevent complete predictability
        xOffset += generateRandomOffset(-15.0, 15.0);
        
        spawnSingleObstacle(def.obstacleTypes[i], xOffset);
    }
    
    // Reset pattern cooldown for complex patterns
    if (def.patternDifficulty > 0.6) {
        patternCooldownTimer = 0.0;
    }
}

std::string ObstacleManager::getCurrentPatternName() const {
    auto it = patternDefinitions.find(lastPattern);
    return (it != patternDefinitions.end()) ? it->second.patternName : "Unknown";
}

// ===== Pattern System Implementation =====

void ObstacleManager::initializePatterns() {
    // Clear existing patterns
    patternDefinitions.clear();
    
    // SINGLE_SMALL - Easiest pattern for beginners
    PatternDefinition singleSmall;
    singleSmall.obstacleTypes = {Obstacle::ObstacleType::CACTUS_SMALL};
    singleSmall.relativePositions = {0.0};
    singleSmall.patternDifficulty = 0.1;
    singleSmall.minGameTime = 0.0;
    singleSmall.patternName = "Single Small Cactus";
    patternDefinitions[ObstaclePattern::SINGLE_SMALL] = singleSmall;
    
    // SINGLE_MID - Medium single obstacle
    PatternDefinition singleMid;
    singleMid.obstacleTypes = {Obstacle::ObstacleType::CACTUS_MID};
    singleMid.relativePositions = {0.0};
    singleMid.patternDifficulty = 0.25;
    singleMid.minGameTime = 3.0;
    singleMid.patternName = "Single Medium Cactus";
    patternDefinitions[ObstaclePattern::SINGLE_MID] = singleMid;
    
    // SINGLE_LARGE - Challenging single obstacle
    PatternDefinition singleLarge;
    singleLarge.obstacleTypes = {Obstacle::ObstacleType::CACTUS_LARGE};
    singleLarge.relativePositions = {0.0};
    singleLarge.patternDifficulty = 0.4;
    singleLarge.minGameTime = 8.0;
    singleLarge.patternName = "Single Large Cactus";
    patternDefinitions[ObstaclePattern::SINGLE_LARGE] = singleLarge;
    
    // DOUBLE_CLOSE - Two obstacles requiring precise timing
    PatternDefinition doubleClose;
    doubleClose.obstacleTypes = {Obstacle::ObstacleType::CACTUS_SMALL, Obstacle::ObstacleType::CACTUS_SMALL};
    doubleClose.relativePositions = {0.0, 30.0};  // Close spacing requires skill
    doubleClose.patternDifficulty = 0.65;
    doubleClose.minGameTime = 15.0;
    doubleClose.patternName = "Double Close Cacti";
    patternDefinitions[ObstaclePattern::DOUBLE_CLOSE] = doubleClose;
    
    // TRIPLE_CLUSTER - Three obstacles for expert players
    PatternDefinition tripleCluster;
    tripleCluster.obstacleTypes = {
        Obstacle::ObstacleType::CACTUS_SMALL, 
        Obstacle::ObstacleType::CACTUS_MID, 
        Obstacle::ObstacleType::CACTUS_SMALL
    };
    tripleCluster.relativePositions = {0.0, 30.0, 60.0};
    tripleCluster.patternDifficulty = 0.9;
    tripleCluster.minGameTime = 20.0;
    tripleCluster.patternName = "Triple Cluster";
    patternDefinitions[ObstaclePattern::TRIPLE_CLUSTER] = tripleCluster;
    
    // MIXED_HEIGHTS - Varied obstacle sizes
    PatternDefinition mixedHeights;
    mixedHeights.obstacleTypes = {Obstacle::ObstacleType::CACTUS_LARGE, Obstacle::ObstacleType::CACTUS_SMALL};
    mixedHeights.relativePositions = {0.0, 40.0};
    mixedHeights.patternDifficulty = 0.5;
    mixedHeights.minGameTime = 12.0;
    mixedHeights.patternName = "Mixed Heights";
    patternDefinitions[ObstaclePattern::MIXED_HEIGHTS] = mixedHeights;
    
    // WIDE_GAP - Easy breather pattern
    PatternDefinition wideGap;
    wideGap.obstacleTypes = {Obstacle::ObstacleType::CACTUS_SMALL, Obstacle::ObstacleType::CACTUS_SMALL};
    wideGap.relativePositions = {0.0, 180.0};  // Wide gap for recovery
    wideGap.patternDifficulty = 0.2;
    wideGap.minGameTime = 0.0;
    wideGap.patternName = "Wide Gap";
    patternDefinitions[ObstaclePattern::WIDE_GAP] = wideGap;
    
    // TIGHT_SEQUENCE - Expert level challenge
    PatternDefinition tightSequence;
    tightSequence.obstacleTypes = {
        Obstacle::ObstacleType::CACTUS_SMALL,
        Obstacle::ObstacleType::CACTUS_MID,
        Obstacle::ObstacleType::CACTUS_LARGE
    };
    tightSequence.relativePositions = {0.0, 45.0, 85.0};  // Very tight spacing
    tightSequence.patternDifficulty = 1.0;
    tightSequence.minGameTime = 25.0;
    tightSequence.patternName = "Tight Sequence";
    patternDefinitions[ObstaclePattern::TIGHT_SEQUENCE] = tightSequence;
    
    std::cout << "Initialized " << patternDefinitions.size() << " obstacle patterns" << std::endl;
}

void ObstacleManager::updateAvailablePatterns(double gameTime) {
    availablePatterns.clear();
    
    // Add patterns that are unlocked at current game time
    for (const auto& pair : patternDefinitions) {
        if (gameTime >= pair.second.minGameTime) {
            availablePatterns.push_back(pair.first);
        }
    }
    
    // Ensure at least basic patterns are always available
    if (availablePatterns.empty()) {
        availablePatterns.push_back(ObstaclePattern::SINGLE_SMALL);
    }
}

ObstacleManager::ObstaclePattern ObstacleManager::weightedPatternSelection(double gameTime) const {
    if (availablePatterns.empty()) {
        return ObstaclePattern::SINGLE_SMALL;
    }
    
    // Calculate weights for all available patterns
    std::vector<double> weights;
    double totalWeight = 0.0;
    
    for (ObstaclePattern pattern : availablePatterns) {
        double weight = getPatternWeight(pattern, currentDifficulty);
        weights.push_back(weight);
        totalWeight += weight;
    }
    
    // Select pattern using weighted random selection
    double randomValue = uniformDist(randomGenerator) * totalWeight;
    double accumulatedWeight = 0.0;
    
    for (size_t i = 0; i < availablePatterns.size(); ++i) {
        accumulatedWeight += weights[i];
        if (randomValue <= accumulatedWeight) {
            return availablePatterns[i];
        }
    }
    
    // Fallback to last available pattern
    return availablePatterns.back();
}

bool ObstacleManager::shouldAvoidPattern(ObstaclePattern pattern) const {
    // safety check: ensure pattern exists in definitions
    auto it = patternDefinitions.find(pattern);
    if (it == patternDefinitions.end()) {
        return false;  // never avoid unknown patterns
    }

    const PatternDefinition& def = it->second;
    
    // Avoid hard patterns if we've had too many consecutive hard patterns
    if (def.patternDifficulty > 0.6 && consecutiveHardPatterns >= MAX_CONSECUTIVE_HARD) {
        return true;
    }
    
    // Avoid very complex patterns if cooldown hasn't expired
    if (def.patternDifficulty > 0.8 && patternCooldownTimer < PATTERN_COOLDOWN_TIME) {
        return true;
    }
    
    return false;
}

double ObstacleManager::getPatternWeight(ObstaclePattern pattern, double currentDifficulty) const {
    // 안전한 접근 방식
    auto it = patternDefinitions.find(pattern);
    if (it == patternDefinitions.end()) {
        return 0.1;  // 기본 최소 가중치
    }
    
    const PatternDefinition& def = it->second;
    
    double weight = 1.0;
    double difficultyMatch = 1.0 - std::abs(def.patternDifficulty - currentDifficulty);
    weight *= (0.3 + 0.7 * difficultyMatch);
    
    if (shouldAvoidPattern(pattern)) {
        weight *= 0.1;
    }
    
    return weight;
}

// ===== Helper Methods =====

void ObstacleManager::spawnSingleObstacle(Obstacle::ObstacleType type, double xOffset) {
    // Create obstacle with specified type and position offset
    double offsetY = 0.0;
    if (type == Obstacle::ObstacleType::CACTUS_MID) {
        offsetY -= 13.0;
    } else if (type == Obstacle::ObstacleType::CACTUS_LARGE) {
        offsetY -= 33.0; // Adjust for larger cactus height
    }
    obstacles.emplace_back(SPAWN_POSITION_X + xOffset, SPAWN_POSITION_Y + offsetY, obstacleSpeed, type);
}

void ObstacleManager::spawnObstacle() {
    // Legacy method - now uses pattern system for single obstacles
    spawnPattern(ObstaclePattern::SINGLE_SMALL);
}

void ObstacleManager::removeOffScreenObstacles() {
    // Use erase-remove idiom: C++ standard pattern for efficient element removal
    obstacles.erase(
        std::remove_if(obstacles.begin(), obstacles.end(),
            [](const Obstacle& obs) { return obs.isOffScreen(); }),
        obstacles.end()
    );
}

void ObstacleManager::updateExistingObstacles(double deltaTime) {
    for (auto& obstacle : obstacles) {
        // Apply current speed to obstacle (important for difficulty scaling)
        obstacle.setSpeed(obstacleSpeed);
        
        // Update obstacle position based on its speed and elapsed time
        obstacle.update(deltaTime);
    }
}

void ObstacleManager::applySpeedToAllObstacles() {
    // This method ensures all obstacles move at the current difficulty-adjusted speed
    for (auto& obstacle : obstacles) {
        obstacle.setSpeed(obstacleSpeed);
    }
}

double ObstacleManager::generateRandomOffset(double min, double max) const {
    std::uniform_real_distribution<double> dist(min, max);
    return dist(randomGenerator);
}

Obstacle::ObstacleType ObstacleManager::getRandomObstacleType() const {
    // Use existing obstacle type distribution from Obstacle class
    double random = uniformDist(randomGenerator);
    if (random < 0.4) {
        return Obstacle::ObstacleType::CACTUS_SMALL;
    } else if (random < 0.8) {
        return Obstacle::ObstacleType::CACTUS_MID;
    } else {
        return Obstacle::ObstacleType::CACTUS_LARGE;
    }
}


// Getter methods: Provide read-only access to internal state
const std::vector<Obstacle>& ObstacleManager::getObstacles() const {
    return obstacles;
}

size_t ObstacleManager::getObstacleCount() const {
    return obstacles.size();
}

double ObstacleManager::getCurrentSpeed() const {
    return obstacleSpeed;
}

double ObstacleManager::getCurrentSpawnInterval() const {
    return obstacleInterval;
}

bool ObstacleManager::hasObstacles() const {
    return !obstacles.empty();
}

// Debug method: Calculate and output average distance between obstacles
void ObstacleManager::getAverageObstacleDistance() const {
    if (obstacles.size() < 2) {
        std::cout << "Average obstacle distance: N/A (less than 2 obstacles)" << std::endl;
        return;
    }
    
    double totalDistance = 0.0;
    for (size_t i = 1; i < obstacles.size(); ++i) {
        // Calculate distance between consecutive obstacles
        double distance = obstacles[i-1].getPosX() - obstacles[i].getPosX();
        totalDistance += distance;
    }
    
    double averageDistance = totalDistance / (obstacles.size() - 1);
    std::cout << "Average obstacle distance: " << averageDistance << " pixels" << std::endl;
}