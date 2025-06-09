#include "TextureManager.hpp"
#include <iostream>
#include <filesystem>

// ===== Static Member Definitions =====

std::unique_ptr<TextureManager> TextureManager::instance = nullptr;

// Define sprite file paths - can be easily modified for different asset locations
const std::unordered_map<TextureManager::SpriteType, std::string> TextureManager::SPRITE_PATHS = {
    {SpriteType::DINO_RUNNING_1, "assets/sprites/dino_spritesheet.png"},
    {SpriteType::DINO_RUNNING_2, "assets/sprites/dino_spritesheet.png"},
    {SpriteType::DINO_JUMPING, "assets/sprites/dino_spritesheet.png"},
    {SpriteType::DINO_DUCKING, "assets/sprites/dino_spritesheet.png"},
    {SpriteType::CACTUS_SMALL, "assets/sprites/obstacle_spritesheet.png"},
    {SpriteType::CACTUS_LARGE, "assets/sprites/obstacle_spritesheet.png"},
    {SpriteType::BIRD_FLYING_1, "assets/sprites/bird_spritesheet.png"},
    {SpriteType::BIRD_FLYING_2, "assets/sprites/bird_spritesheet.png"},
    {SpriteType::GROUND, "assets/sprites/ground.png"},
    {SpriteType::CLOUD, "assets/sprites/cloud.png"},
    {SpriteType::BACKGROUND, "assets/sprites/background.png"}
};

// Define sprite rectangles within sprite sheets
// These coordinates define where each sprite is located in the sprite sheet
const std::unordered_map<TextureManager::SpriteType, sf::IntRect> TextureManager::DEFAULT_SPRITE_RECTS = {
    // Dino sprites (assuming 44x47 pixel sprites in a sprite sheet)
    {SpriteType::DINO_RUNNING_1, sf::IntRect(0, 0, 44, 47)},
    {SpriteType::DINO_RUNNING_2, sf::IntRect(44, 0, 44, 47)},
    {SpriteType::DINO_JUMPING, sf::IntRect(88, 0, 44, 47)},
    {SpriteType::DINO_DUCKING, sf::IntRect(132, 0, 59, 30)},  // Ducking sprite is wider and shorter
    
    // Cactus sprites (assuming different sizes)
    {SpriteType::CACTUS_SMALL, sf::IntRect(0, 0, 17, 35)},
    {SpriteType::CACTUS_LARGE, sf::IntRect(17, 0, 25, 50)},
    
    // Bird sprites (assuming 46x40 pixel sprites)
    {SpriteType::BIRD_FLYING_1, sf::IntRect(0, 0, 46, 40)},
    {SpriteType::BIRD_FLYING_2, sf::IntRect(46, 0, 46, 40)},
    
    // Environment sprites (full textures)
    {SpriteType::GROUND, sf::IntRect(0, 0, 2400, 12)},  // Long ground texture for scrolling
    {SpriteType::CLOUD, sf::IntRect(0, 0, 46, 14)},
    {SpriteType::BACKGROUND, sf::IntRect(0, 0, 800, 600)}  // Full screen background
};

// ===== Core Lifecycle Methods =====

TextureManager& TextureManager::getInstance() {
    if (!instance) {
        instance = std::unique_ptr<TextureManager>(new TextureManager());
    }
    return *instance;
}

bool TextureManager::initialize() {
    std::cout << "=== TextureManager Initialization ===" << std::endl;
    
    // First, try to load actual sprite files
    bool spritesLoaded = loadDefaultTextures();
    
    if (!spritesLoaded) {
        std::cout << "Warning: Could not load sprite files. Creating fallback textures..." << std::endl;
        
        // Create fallback colored rectangles that match our original design
        auto dinoTexture = createFallbackTexture(sf::Color::Green, sf::Vector2u(44, 47));
        auto obstacleTexture = createFallbackTexture(sf::Color::Red, sf::Vector2u(25, 50));
        auto backgroundTexture = createFallbackTexture(sf::Color::White, sf::Vector2u(800, 600));
        
        if (dinoTexture && obstacleTexture && backgroundTexture) {
            textures["dino_fallback"] = std::move(dinoTexture);
            textures["obstacle_fallback"] = std::move(obstacleTexture);
            textures["background_fallback"] = std::move(backgroundTexture);
            
            // Set up fallback sprite rectangles (full texture for each)
            spriteRects[SpriteType::DINO_RUNNING_1] = sf::IntRect(0, 0, 44, 47);
            spriteRects[SpriteType::DINO_RUNNING_2] = sf::IntRect(0, 0, 44, 47);
            spriteRects[SpriteType::DINO_JUMPING] = sf::IntRect(0, 0, 44, 47);
            spriteRects[SpriteType::CACTUS_SMALL] = sf::IntRect(0, 0, 25, 50);
            spriteRects[SpriteType::CACTUS_LARGE] = sf::IntRect(0, 0, 25, 50);
            spriteRects[SpriteType::BACKGROUND] = sf::IntRect(0, 0, 800, 600);
            
            std::cout << "Fallback textures created successfully." << std::endl;
            return true;
        } else {
            std::cerr << "Error: Failed to create fallback textures!" << std::endl;
            return false;
        }
    }
    
    std::cout << "TextureManager initialized with " << textures.size() << " textures." << std::endl;
    return true;
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

sf::Sprite TextureManager::createSprite(SpriteType spriteType, float scale) const {
    sf::Sprite sprite = createSprite(spriteType);
    sprite.setScale(scale, scale);
    return sprite;
}

sf::Sprite TextureManager::createSprite(SpriteType spriteType, const sf::Vector2f& targetSize) const {
    sf::Sprite sprite = createSprite(spriteType);
    
    // Calculate scale based on target size
    sf::IntRect spriteRect = getSpriteRect(spriteType);
    float scaleX = targetSize.x / spriteRect.width;
    float scaleY = targetSize.y / spriteRect.height;
    
    sprite.setScale(scaleX, scaleY);
    return sprite;
}

// ===== Utility Methods =====

bool TextureManager::isTextureLoaded(const std::string& name) const {
    return textures.find(name) != textures.end();
}

sf::Vector2f TextureManager::getSpriteSize(SpriteType spriteType) const {
    sf::IntRect rect = getSpriteRect(spriteType);
    return sf::Vector2f(static_cast<float>(rect.width), static_cast<float>(rect.height));
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

std::string TextureManager::getTextureNameForSprite(SpriteType spriteType) const {
    // Map sprite types to their texture names
    switch (spriteType) {
        case SpriteType::DINO_RUNNING_1:
        case SpriteType::DINO_RUNNING_2:
        case SpriteType::DINO_JUMPING:
        case SpriteType::DINO_DUCKING:
            return textures.find("dino_spritesheet") != textures.end() ? "dino_spritesheet" : "dino_fallback";
            
        case SpriteType::CACTUS_SMALL:
        case SpriteType::CACTUS_LARGE:
            return textures.find("obstacle_spritesheet") != textures.end() ? "obstacle_spritesheet" : "obstacle_fallback";
            
        case SpriteType::BIRD_FLYING_1:
        case SpriteType::BIRD_FLYING_2:
            return textures.find("bird_spritesheet") != textures.end() ? "bird_spritesheet" : "obstacle_fallback";
            
        case SpriteType::BACKGROUND:
            return textures.find("background") != textures.end() ? "background" : "background_fallback";
            
        case SpriteType::GROUND:
            return "ground";
            
        case SpriteType::CLOUD:
            return "cloud";
            
        default:
            return "dino_fallback";  // Safe fallback
    }
}

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