#include "TextureManager.hpp"
#include <iostream>

// ===== Static Member Definitions =====

std::unique_ptr<TextureManager> TextureManager::instance = nullptr;

// Updated sprite file paths for your actual sprite sheets
const std::unordered_map<TextureManager::SpriteType, std::string> TextureManager::SPRITE_PATHS = {
    {SpriteType::DINO_RUNNING_1, "assets/sprites/dino_sheet.png"},
    {SpriteType::DINO_RUNNING_2, "assets/sprites/dino_sheet.png"},
    {SpriteType::DINO_JUMPING, "assets/sprites/dino_sheet.png"},
    {SpriteType::DINO_DUCKING_1, "assets/sprites/dino_sheet.png"},
    {SpriteType::DINO_DUCKING_2, "assets/sprites/dino_sheet.png"},
    {SpriteType::CACTUS_SMALL, "assets/sprites/obstacles_sheet.png"},
    {SpriteType::CACTUS_MID, "assets/sprites/obstacles_sheet.png"},
    {SpriteType::CACTUS_LARGE, "assets/sprites/obstacles_sheet.png"},
    {SpriteType::BIRD_FLYING_1, "assets/sprites/obstacles_sheet.png"},
    {SpriteType::BIRD_FLYING_2, "assets/sprites/obstacles_sheet.png"},
    {SpriteType::CLOUD, "assets/sprites/obstacles_sheet.png"}
};

// Precise sprite rectangles based on your actual sprite sheets
const std::unordered_map<TextureManager::SpriteType, sf::IntRect> TextureManager::DEFAULT_SPRITE_RECTS = {
    // Dino sprites - precise coordinates for Chrome Dino sprite sheet
    {SpriteType::DINO_RUNNING_1, sf::IntRect(298, 58, 80, 86)},
    {SpriteType::DINO_RUNNING_2, sf::IntRect(394, 58, 80, 86)},
    {SpriteType::DINO_JUMPING, sf::IntRect(106, 58, 80, 86)},
    {SpriteType::DINO_DUCKING_1, sf::IntRect(681, 92, 110, 53)},
    {SpriteType::DINO_DUCKING_2, sf::IntRect(807, 92, 110, 53)},
    
    // Birds from obstacles sheet
    {SpriteType::BIRD_FLYING_1, sf::IntRect(134, 15, 46, 40)},
    {SpriteType::BIRD_FLYING_2, sf::IntRect(180, 15, 46, 40)},
    
    // Cacti from obstacles sheet
    {SpriteType::CACTUS_SMALL, sf::IntRect(48, 150, 18, 35)},
    {SpriteType::CACTUS_MID, sf::IntRect(167, 136, 25, 48)},
    {SpriteType::CACTUS_LARGE, sf::IntRect(47, 228, 32, 68)},
    
    // Cloud decoration
    {SpriteType::CLOUD, sf::IntRect(86, 2, 46, 14)}
};

// ===== Core Methods =====

TextureManager& TextureManager::getInstance() {
    if (!instance) {
        instance = std::unique_ptr<TextureManager>(new TextureManager());
    }
    return *instance;
}

bool TextureManager::initialize() {
    std::cout << "=== TextureManager: Loading Chrome Dino Sprites ===" << std::endl;
    
    // Try to load the two main sprite sheets with multiple paths
    bool dinoLoaded = false;
    bool obstaclesLoaded = false;
    
    // Try multiple paths for dino sheet
    std::vector<std::string> dinoPaths = {
        "../assets/sprites/dino_sheet.png",       // bin/ 폴더에서 실행
    };
    
    std::cout << "Attempting to load dino sprite sheet..." << std::endl;
    for (const std::string& path : dinoPaths) {
        std::cout << "  Trying: " << path << std::endl;
        if (loadTexture("dino_sheet", path)) {
            dinoLoaded = true;
            std::cout << "  SUCCESS!" << std::endl;
            break;
        }
    }
    
    // Try multiple paths for obstacles sheet
    std::vector<std::string> obstaclesPaths = {
        "../assets/sprites/obstacles_sheet.png",
    };
    
    std::cout << "Attempting to load obstacles sprite sheet..." << std::endl;
    for (const std::string& path : obstaclesPaths) {
        std::cout << "  Trying: " << path << std::endl;
        if (loadTexture("obstacles_sheet", path)) {
            obstaclesLoaded = true;
            std::cout << "  SUCCESS!" << std::endl;
            break;
        }
    }
    
    if (dinoLoaded && obstaclesLoaded) {
        spriteRects = DEFAULT_SPRITE_RECTS;
        std::cout << "All sprite sheets loaded successfully!" << std::endl;
        printDebugInfo();
        return true;
    } else {
        std::cout << "Some sprite sheets failed to load. Creating fallbacks..." << std::endl;
        std::cout << "Dino loaded: " << (dinoLoaded ? "YES" : "NO") << std::endl;
        std::cout << "Obstacles loaded: " << (obstaclesLoaded ? "YES" : "NO") << std::endl;
        return createFallbackTextures();
    }
}

bool TextureManager::createFallbackTextures() {
    std::cout << "Creating enhanced fallback textures..." << std::endl;
    
    // Create fallback colored rectangles with improved appearance
    auto dinoTexture = createFallbackTexture(sf::Color::Green, sf::Vector2u(44, 47));
    auto obstacleTexture = createFallbackTexture(sf::Color::Red, sf::Vector2u(25, 50));
    auto birdTexture = createFallbackTexture(sf::Color::Blue, sf::Vector2u(46, 40));
    
    if (dinoTexture && obstacleTexture && birdTexture) {
        textures["dino_fallback"] = std::move(dinoTexture);
        textures["obstacle_fallback"] = std::move(obstacleTexture);
        textures["bird_fallback"] = std::move(birdTexture);
        
        // Set up fallback sprite rectangles for collision control
        spriteRects[SpriteType::DINO_RUNNING_1] = sf::IntRect(0, 0, 80, 86);
        spriteRects[SpriteType::DINO_RUNNING_2] = sf::IntRect(0, 0, 80, 86);
        spriteRects[SpriteType::DINO_JUMPING] = sf::IntRect(0, 0, 80, 86);
        spriteRects[SpriteType::DINO_DUCKING_1] = sf::IntRect(0, 0, 110, 53);
        spriteRects[SpriteType::DINO_DUCKING_2] = sf::IntRect(0, 0, 110, 53);
        spriteRects[SpriteType::CACTUS_SMALL] = sf::IntRect(0, 0, 15, 35);
        spriteRects[SpriteType::CACTUS_MID] = sf::IntRect(0, 0, 25, 48);
        spriteRects[SpriteType::CACTUS_LARGE] = sf::IntRect(0, 0, 32, 68);
        spriteRects[SpriteType::BIRD_FLYING_1] = sf::IntRect(0, 0, 46, 40);
        spriteRects[SpriteType::BIRD_FLYING_2] = sf::IntRect(0, 0, 46, 40);
        
        std::cout << "Fallback textures created successfully." << std::endl;
        return true;
    }
    
    std::cerr << "ERROR: Failed to create fallback textures!" << std::endl;
    return false;
}

void TextureManager::cleanup() {
    textures.clear();
    spriteRects.clear();
    std::cout << "TextureManager cleaned up." << std::endl;
}

// ===== Texture Loading Methods =====

bool TextureManager::loadTexture(const std::string& name, const std::string& filepath) {
    if (textures.find(name) != textures.end()) {
        return true;
    }
    
    auto texture = std::make_unique<sf::Texture>();
    
    if (!texture->loadFromFile(filepath)) {
        return false;
    }
    
    textures[name] = std::move(texture);
    std::cout << "    Loaded: " << name << " from " << filepath << std::endl;
    return true;
}

bool TextureManager::loadSpriteSheet(const std::string& name, const std::string& filepath,
                                   const std::unordered_map<SpriteType, sf::IntRect>& spriteDefinitions) {
    if (!loadTexture(name, filepath)) {
        return false;
    }
    
    // C++14 compatible loop
    for (const auto& spriteDefPair : spriteDefinitions) {
        spriteRects[spriteDefPair.first] = spriteDefPair.second;
    }
    
    std::cout << "Loaded sprite sheet: " << name << " with " << spriteDefinitions.size() << " sprites." << std::endl;
    return true;
}

// ===== Texture Access Methods =====

const sf::Texture* TextureManager::getTexture(const std::string& name) const {
    auto it = textures.find(name);
    return (it != textures.end()) ? it->second.get() : nullptr;
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
    
    auto defaultIt = DEFAULT_SPRITE_RECTS.find(spriteType);
    if (defaultIt != DEFAULT_SPRITE_RECTS.end()) {
        return defaultIt->second;
    }
    
    return sf::IntRect(0, 0, 32, 32);  // Safe fallback
}

// ===== Sprite Creation Methods =====

sf::Sprite TextureManager::createSprite(SpriteType spriteType) const {
    sf::Sprite sprite;
    
    const sf::Texture* texture = getTexture(spriteType);
    if (texture) {
        sprite.setTexture(*texture);
        sprite.setTextureRect(getSpriteRect(spriteType));
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
    
    // C++14 compatible loop
    for (const auto& texturePair : textures) {
        sf::Vector2u size = texturePair.second->getSize();
        std::cout << "  - " << texturePair.first << ": " << size.x << "x" << size.y << " pixels" << std::endl;
    }
    
    std::cout << "Defined sprite rectangles: " << spriteRects.size() << std::endl;
    std::cout << "================================\n" << std::endl;
}

// ===== Helper Methods =====

std::string TextureManager::getTextureNameForSprite(SpriteType spriteType) const {
    switch (spriteType) {
        case SpriteType::DINO_RUNNING_1:
        case SpriteType::DINO_RUNNING_2:
        case SpriteType::DINO_JUMPING:
        case SpriteType::DINO_DUCKING_1:
        case SpriteType::DINO_DUCKING_2:
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

std::unique_ptr<sf::Texture> TextureManager::createFallbackTexture(sf::Color color, sf::Vector2u size) {
    auto texture = std::make_unique<sf::Texture>();
    
    sf::Image image;
    image.create(size.x, size.y, color);
    
    // Add visual enhancements for better appearance
    if (color == sf::Color::Green) {
        // Dino-like outline
        for (unsigned int y = 0; y < size.y; ++y) {
            for (unsigned int x = 0; x < size.x; ++x) {
                if (x == 0 || x == size.x-1 || y == 0 || y == size.y-1) {
                    image.setPixel(x, y, sf::Color::Black);
                }
            }
        }
    } else if (color == sf::Color::Red) {
        // Cactus-like outline
        for (unsigned int y = 0; y < size.y; ++y) {
            for (unsigned int x = 0; x < size.x; ++x) {
                if (x == 0 || x == size.x-1 || y == 0 || y == size.y-1) {
                    image.setPixel(x, y, sf::Color::Black);
                }
            }
        }
    }
    
    if (!texture->loadFromImage(image)) {
        return nullptr;
    }
    
    return texture;
}