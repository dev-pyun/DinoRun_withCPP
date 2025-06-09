#include "TextureManager.hpp"
#include <iostream>
#include <filesystem>

// ===== Static Member Definitions =====

std::unique_ptr<TextureManager> TextureManager::instance = nullptr;

// Updated sprite file paths for your actual sprite sheets
const std::unordered_map<TextureManager::SpriteType, std::string> TextureManager::SPRITE_PATHS = {
    // All dino sprites from the first image
    {SpriteType::DINO_RUNNING_1, "assets/sprites/dino_sheet.png"},
    {SpriteType::DINO_RUNNING_2, "assets/sprites/dino_sheet.png"},
    {SpriteType::DINO_JUMPING, "assets/sprites/dino_sheet.png"},
    {SpriteType::DINO_DUCKING, "assets/sprites/dino_sheet.png"},
    
    // Obstacles and environment from the second image
    {SpriteType::CACTUS_SMALL, "assets/sprites/obstacles_sheet.png"},
    {SpriteType::CACTUS_LARGE, "assets/sprites/obstacles_sheet.png"},
    {SpriteType::BIRD_FLYING_1, "assets/sprites/obstacles_sheet.png"},
    {SpriteType::BIRD_FLYING_2, "assets/sprites/obstacles_sheet.png"},
    {SpriteType::CLOUD, "assets/sprites/obstacles_sheet.png"}
};

// Precise sprite rectangles based on your actual sprite sheets
const std::unordered_map<TextureManager::SpriteType, sf::IntRect> TextureManager::DEFAULT_SPRITE_RECTS = {
    // Dino sprites from first row of dino_sheet.png (analyzing the first image)
    {SpriteType::DINO_RUNNING_1, sf::IntRect(76, 6, 44, 47)},    // First running frame
    {SpriteType::DINO_RUNNING_2, sf::IntRect(132, 6, 44, 47)},   // Second running frame
    {SpriteType::DINO_JUMPING, sf::IntRect(188, 6, 44, 47)},     // Jumping frame
    
    // Ducking sprites (wider and shorter)
    {SpriteType::DINO_DUCKING, sf::IntRect(262, 19, 59, 30)},    // Ducking frame
    
    // Birds from obstacles_sheet.png (top of second image)
    {SpriteType::BIRD_FLYING_1, sf::IntRect(134, 15, 46, 40)},   // First bird frame
    {SpriteType::BIRD_FLYING_2, sf::IntRect(180, 15, 46, 40)},   // Second bird frame
    
    // Cacti from obstacles_sheet.png
    {SpriteType::CACTUS_SMALL, sf::IntRect(228, 30, 17, 35)},    // Small cactus
    {SpriteType::CACTUS_LARGE, sf::IntRect(245, 15, 25, 50)},    // Large cactus
    
    // Cloud for decoration
    {SpriteType::CLOUD, sf::IntRect(86, 2, 46, 14)}             // Cloud sprite
};


// ===== Core Lifecycle Methods =====

TextureManager& TextureManager::getInstance() {
    if (!instance) {
        instance = std::unique_ptr<TextureManager>(new TextureManager());
    }
    return *instance;
}

bool TextureManager::initialize() {
    std::cout << "=== TextureManager: Loading Chrome Dino Sprites ===" << std::endl;
    
    // Load the two main sprite sheets
    bool dinoSheetLoaded = loadTexture("dino_sheet", "assets/sprites/dino_sheet.png");
    bool obstaclesSheetLoaded = loadTexture("obstacles_sheet", "assets/sprites/obstacles_sheet.png");
    
    if (dinoSheetLoaded && obstaclesSheetLoaded) {
        // Set up sprite rectangles for successful loads
        spriteRects = DEFAULT_SPRITE_RECTS;
        std::cout << "All sprite sheets loaded successfully!" << std::endl;
        return true;
    } else {
        std::cout << "Some sprite sheets failed to load. Creating fallbacks..." << std::endl;
        return createFallbackTextures();
    }
}


bool TextureManager::createFallbackTextures() {
    // Create fallback colored rectangles
    auto dinoTexture = createFallbackTexture(sf::Color::Green, sf::Vector2u(44, 47));
    auto obstacleTexture = createFallbackTexture(sf::Color::Red, sf::Vector2u(25, 50));
    auto birdTexture = createFallbackTexture(sf::Color::Blue, sf::Vector2u(46, 40));
    
    if (dinoTexture && obstacleTexture && birdTexture) {
        textures["dino_fallback"] = std::move(dinoTexture);
        textures["obstacle_fallback"] = std::move(obstacleTexture);
        textures["bird_fallback"] = std::move(birdTexture);
        
        // Set up fallback sprite rectangles (full texture for each)
        spriteRects[SpriteType::DINO_RUNNING_1] = sf::IntRect(0, 0, 44, 47);
        spriteRects[SpriteType::DINO_RUNNING_2] = sf::IntRect(0, 0, 44, 47);
        spriteRects[SpriteType::DINO_JUMPING] = sf::IntRect(0, 0, 44, 47);
        spriteRects[SpriteType::DINO_DUCKING] = sf::IntRect(0, 0, 59, 30);
        spriteRects[SpriteType::CACTUS_SMALL] = sf::IntRect(0, 0, 17, 35);
        spriteRects[SpriteType::CACTUS_LARGE] = sf::IntRect(0, 0, 25, 50);
        spriteRects[SpriteType::BIRD_FLYING_1] = sf::IntRect(0, 0, 46, 40);
        spriteRects[SpriteType::BIRD_FLYING_2] = sf::IntRect(0, 0, 46, 40);
        
        std::cout << "Fallback textures created successfully." << std::endl;
        return true;
    }
    
    std::cerr << "Error: Failed to create fallback textures!" << std::endl;
    return false;
}

std::string TextureManager::getTextureNameForSprite(SpriteType spriteType) const {
    switch (spriteType) {
        case SpriteType::DINO_RUNNING_1:
        case SpriteType::DINO_RUNNING_2:
        case SpriteType::DINO_JUMPING:
        case SpriteType::DINO_DUCKING:
            return textures.find("dino_sheet") != textures.end() ? "dino_sheet" : "dino_fallback";
            
        case SpriteType::CACTUS_SMALL:
        case SpriteType::CACTUS_LARGE:
        case SpriteType::BIRD_FLYING_1:
        case SpriteType::BIRD_FLYING_2:
        case SpriteType::CLOUD:
            return textures.find("obstacles_sheet") != textures.end() ? "obstacles_sheet" : "obstacle_fallback";
            
        default:
            return "dino_fallback";
    }
}

void TextureManager::cleanup() {
    textures.clear();
    spriteRects.clear();
    std::cout << "TextureManager cleaned up." << std::endl;
}

// ===== Texture Loading Methods =====

bool TextureManager::loadTexture(const std::string& name, const std::string& filepath) {
    // Check if texture is already loaded
    if (textures.find(name) != textures.end()) {
        std::cout << "Texture '" << name << "' already loaded." << std::endl;
        return true;
    }
    
    // Create new texture
    auto texture = std::make_unique<sf::Texture>();
    
    if (!texture->loadFromFile(filepath)) {
        std::cerr << "Error: Failed to load texture from '" << filepath << "'" << std::endl;
        return false;
    }
    
    // Store the texture
    textures[name] = std::move(texture);
    std::cout << "Loaded texture: " << name << " from " << filepath << std::endl;
    return true;
}

bool TextureManager::loadSpriteSheet(const std::string& name, const std::string& filepath,
                                   const std::unordered_map<SpriteType, sf::IntRect>& spriteDefinitions) {
    // Load the sprite sheet texture
    if (!loadTexture(name, filepath)) {
        return false;
    }
    
    // Store sprite rectangle definitions
    for (const auto& [spriteType, rect] : spriteDefinitions) {
        spriteRects[spriteType] = rect;
    }
    
    std::cout << "Loaded sprite sheet: " << name << " with " << spriteDefinitions.size() << " sprites." << std::endl;
    return true;
}

// ===== Texture Access Methods =====

const sf::Texture* TextureManager::getTexture(const std::string& name) const {
    auto it = textures.find(name);
    if (it != textures.end()) {
        return it->second.get();
    }
    
    std::cerr << "Warning: Texture '" << name << "' not found!" << std::endl;
    return nullptr;
}

const sf::Texture* TextureManager::getTexture(SpriteType spriteType) const {
    std::string textureName = getTextureNameForSprite(spriteType);
    return getTexture(textureName);
}

sf::IntRect TextureManager::getSpriteRect(SpriteType spriteType) const {
    auto it = spriteRects.find(spriteType);
    if (it != spriteRects.end()) {
        return it->second;
    }
    
    // Return default rectangle if not found
    auto defaultIt = DEFAULT_SPRITE_RECTS.find(spriteType);
    if (defaultIt != DEFAULT_SPRITE_RECTS.end()) {
        return defaultIt->second;
    }
    
    std::cerr << "Warning: Sprite rectangle for type " << static_cast<int>(spriteType) << " not found!" << std::endl;
    return sf::IntRect(0, 0, 32, 32);  // Fallback rectangle
}

// ===== Sprite Creation Methods =====

sf::Sprite TextureManager::createSprite(SpriteType spriteType) const {
    sf::Sprite sprite;
    
    const sf::Texture* texture = getTexture(spriteType);
    if (texture) {
        sprite.setTexture(*texture);
        sprite.setTextureRect(getSpriteRect(spriteType));
    } else {
        std::cerr << "Warning: Could not create sprite for type " << static_cast<int>(spriteType) << std::endl;
    }
    
    return sprite;
}

sf::Sprite TextureManager::createSprite(SpriteType spriteType, double scale) const {
    sf::Sprite sprite = createSprite(spriteType);
    sprite.setScale(scale, scale);
    return sprite;
}

sf::Sprite TextureManager::createSprite(SpriteType spriteType, const sf::Vector2f& targetSize) const {
    sf::Sprite sprite = createSprite(spriteType);
    
    // Calculate scale based on target size
    sf::IntRect spriteRect = getSpriteRect(spriteType);
    double scaleX = targetSize.x / spriteRect.width;
    double scaleY = targetSize.y / spriteRect.height;
    
    sprite.setScale(scaleX, scaleY);
    return sprite;
}

// ===== Utility Methods =====

bool TextureManager::isTextureLoaded(const std::string& name) const {
    return textures.find(name) != textures.end();
}

sf::Vector2f TextureManager::getSpriteSize(SpriteType spriteType) const {
    sf::IntRect rect = getSpriteRect(spriteType);
    return sf::Vector2f(static_cast<double>(rect.width), static_cast<double>(rect.height));
}

size_t TextureManager::getLoadedTextureCount() const {
    return textures.size();
}

void TextureManager::printDebugInfo() const {
    std::cout << "\n=== TextureManager Debug Info ===" << std::endl;
    std::cout << "Loaded textures: " << textures.size() << std::endl;
    
    for (const auto& [name, texture] : textures) {
        sf::Vector2u size = texture->getSize();
        std::cout << "  - " << name << ": " << size.x << "x" << size.y << " pixels" << std::endl;
    }
    
    std::cout << "Defined sprite rectangles: " << spriteRects.size() << std::endl;
    std::cout << "================================\n" << std::endl;
}

// ===== Private Helper Methods =====

// std::string TextureManager::getTextureNameForSprite(SpriteType spriteType) const {
//     // Map sprite types to their texture names
//     switch (spriteType) {
//         case SpriteType::DINO_RUNNING_1:
//         case SpriteType::DINO_RUNNING_2:
//         case SpriteType::DINO_JUMPING:
//         case SpriteType::DINO_DUCKING:
//             return textures.find("dino_spritesheet") != textures.end() ? "dino_spritesheet" : "dino_fallback";
            
//         case SpriteType::CACTUS_SMALL:
//         case SpriteType::CACTUS_LARGE:
//             return textures.find("obstacle_spritesheet") != textures.end() ? "obstacle_spritesheet" : "obstacle_fallback";
            
//         case SpriteType::BIRD_FLYING_1:
//         case SpriteType::BIRD_FLYING_2:
//             return textures.find("bird_spritesheet") != textures.end() ? "bird_spritesheet" : "obstacle_fallback";
            
//         case SpriteType::BACKGROUND:
//             return textures.find("background") != textures.end() ? "background" : "background_fallback";
            
//         case SpriteType::GROUND:
//             return "ground";
            
//         case SpriteType::CLOUD:
//             return "cloud";
            
//         default:
//             return "dino_fallback";  // Safe fallback
//     }
// }

bool TextureManager::loadDefaultTextures() {
    bool allLoaded = true;
    
    // List of unique texture files to load
    std::vector<std::pair<std::string, std::string>> textureFiles = {
        {"dino_spritesheet", "assets/sprites/dino_spritesheet.png"},
        {"obstacle_spritesheet", "assets/sprites/obstacle_spritesheet.png"},
        {"bird_spritesheet", "assets/sprites/bird_spritesheet.png"},
        {"ground", "assets/sprites/ground.png"},
        {"cloud", "assets/sprites/cloud.png"},
        {"background", "assets/sprites/background.png"}
    };
    
    // Try to load each texture file
    for (const auto& [name, filepath] : textureFiles) {
        if (!loadTexture(name, filepath)) {
            std::cout << "Could not load " << filepath << ", will use fallback." << std::endl;
            allLoaded = false;
        }
    }
    
    // If textures loaded successfully, set up sprite rectangles
    if (allLoaded) {
        spriteRects = DEFAULT_SPRITE_RECTS;
    }
    
    return allLoaded;
}

std::unique_ptr<sf::Texture> TextureManager::createFallbackTexture(sf::Color color, sf::Vector2u size) {
    auto texture = std::make_unique<sf::Texture>();
    
    // Create an image with the specified color
    sf::Image image;
    image.create(size.x, size.y, color);
    
    // Load the image into the texture
    if (!texture->loadFromImage(image)) {
        std::cerr << "Error: Failed to create fallback texture!" << std::endl;
        return nullptr;
    }
    
    return texture;
}