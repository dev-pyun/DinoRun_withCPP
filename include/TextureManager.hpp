#ifndef TEXTURE_MANAGER_HPP
#define TEXTURE_MANAGER_HPP

#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <string>
#include <memory>

/**
 * TextureManager class: Centralized texture and sprite management system
 * 
 * Design Philosophy:
 * - Singleton pattern for global texture resource management
 * - Efficient memory usage through texture sharing and caching
 * - Support for sprite sheets and individual texture files
 * - Automatic resource cleanup and error handling
 * - Easy integration with existing game objects
 * 
 * This class transforms our rectangle-based game into a visually rich
 * sprite-based experience while maintaining the same clean interfaces
 * that our existing Player and Obstacle classes use.
 */
class TextureManager {
public:
    /**
     * Enumeration for game sprite types
     * Makes it easy to reference specific sprites throughout the game
     */
    enum class SpriteType {
        DINO_RUNNING_1,     // First frame of running animation
        DINO_RUNNING_2,     // Second frame of running animation  
        DINO_JUMPING,       // Jumping pose
        DINO_DUCKING_1,       // Ducking pose (for future expansion)
        DINO_DUCKING_2,       // Ducking pose (for future expansion)
        CACTUS_SMALL,       // Small cactus obstacle
        CACTUS_MID,
        CACTUS_LARGE,       // Large cactus obstacle
        BIRD_FLYING_1,      // First frame of bird animation
        BIRD_FLYING_2,      // Second frame of bird animation
        GROUND,             // Ground texture
        CLOUD,              // Cloud decoration
        BACKGROUND          // Background image
    };

private:
    // ===== Core Resource Storage =====
    std::unordered_map<std::string, std::unique_ptr<sf::Texture>> textures;
    std::unordered_map<SpriteType, sf::IntRect> spriteRects;
    
    // ===== Texture Paths Configuration =====
    static const std::unordered_map<SpriteType, std::string> SPRITE_PATHS;
    static const std::unordered_map<SpriteType, sf::IntRect> DEFAULT_SPRITE_RECTS;
    
    // ===== Singleton Implementation =====
    static std::unique_ptr<TextureManager> instance;
    TextureManager() = default;  // Private constructor

public:
    // ===== Singleton Access =====
    
    /**
     * Get the global TextureManager instance
     * Creates instance on first call (lazy initialization)
     * 
     * @return Reference to the singleton TextureManager
     */
    static TextureManager& getInstance();
    
    /**
     * Initialize the texture manager with all game sprites
     * Call this once at game startup to load all required textures
     * 
     * @return true if all textures loaded successfully, false otherwise
     */
    bool initialize();
    
    /**
     * Clean up all loaded textures and reset the manager
     * Call this when shutting down the game
     */
    void cleanup();
    
    // ===== Texture Loading Methods =====
    
    /**
     * Load a single texture from file
     * Stores texture in cache for future use
     * 
     * @param name Unique identifier for the texture
     * @param filepath Path to the texture file
     * @return true if loaded successfully, false otherwise
     */
    bool loadTexture(const std::string& name, const std::string& filepath);
    
    /**
     * Load a sprite sheet and define sprite rectangles
     * Useful for loading multiple sprites from a single image file
     * 
     * @param name Unique identifier for the sprite sheet
     * @param filepath Path to the sprite sheet file
     * @param spriteDefinitions Map of sprite types to their rectangles in the sheet
     * @return true if loaded successfully, false otherwise
     */
    bool loadSpriteSheet(const std::string& name, const std::string& filepath,
                        const std::unordered_map<SpriteType, sf::IntRect>& spriteDefinitions);
    
    // ===== Texture Access Methods =====
    
    /**
     * Get a texture by name
     * Returns nullptr if texture not found
     * 
     * @param name Texture identifier
     * @return Pointer to texture, or nullptr if not found
     */
    const sf::Texture* getTexture(const std::string& name) const;
    
    /**
     * Get a texture for a specific sprite type
     * Automatically resolves the correct texture based on sprite type
     * 
     * @param spriteType The type of sprite needed
     * @return Pointer to texture, or nullptr if not found
     */
    const sf::Texture* getTexture(SpriteType spriteType) const;
    
    /**
     * Get the texture rectangle for a specific sprite type
     * Returns the portion of the texture that contains the sprite
     * 
     * @param spriteType The type of sprite
     * @return Rectangle defining the sprite area in the texture
     */
    sf::IntRect getSpriteRect(SpriteType spriteType) const;
    
    // ===== Sprite Creation Methods =====
    
    /**
     * Create a configured sprite for a specific type
     * Returns a ready-to-use sprite with correct texture and rectangle
     * 
     * @param spriteType The type of sprite to create
     * @return Configured sf::Sprite object
     */
    sf::Sprite createSprite(SpriteType spriteType) const;
    
    /**
     * Create a sprite with custom scaling
     * Useful for resizing sprites to match game object dimensions
     * 
     * @param spriteType The type of sprite to create
     * @param scale Scaling factor (1.0 = original size)
     * @return Configured and scaled sf::Sprite object
     */
    sf::Sprite createSprite(SpriteType spriteType, double scale) const;
    
    /**
     * Create a sprite with custom size
     * Automatically calculates scaling to match desired size
     * 
     * @param spriteType The type of sprite to create
     * @param targetSize Desired size in pixels
     * @return Configured and sized sf::Sprite object
     */
    sf::Sprite createSprite(SpriteType spriteType, const sf::Vector2f& targetSize) const;
    
    // ===== Utility Methods =====
    
    /**
     * Check if a texture is loaded
     * 
     * @param name Texture identifier
     * @return true if texture exists in cache
     */
    bool isTextureLoaded(const std::string& name) const;
    
    /**
     * Get the size of a specific sprite
     * 
     * @param spriteType The sprite type to check
     * @return Size of the sprite in pixels
     */
    sf::Vector2f getSpriteSize(SpriteType spriteType) const;
    
    /**
     * Get information about loaded textures
     * Useful for debugging and memory usage monitoring
     * 
     * @return Number of textures currently loaded
     */
    size_t getLoadedTextureCount() const;
    
    /**
     * Print debug information about loaded textures
     * Shows memory usage and loaded texture list
     */
    void printDebugInfo() const;

private:
    // ===== Helper Methods =====
    
    /**
     * Get the texture name associated with a sprite type
     * Maps sprite types to their source texture files
     * 
     * @param spriteType The sprite type
     * @return Name of the texture containing this sprite
     */
    std::string getTextureNameForSprite(SpriteType spriteType) const;
    
    /**
     * Load default textures and sprite definitions
     * Called during initialization to set up core game sprites
     * 
     * @return true if all default textures loaded successfully
     */
    bool loadDefaultTextures();
    
    /**
     * Create a colored rectangle texture as fallback
     * Used when sprite files are not available
     * 
     * @param color Color for the fallback rectangle
     * @param size Size of the rectangle
     * @return Unique pointer to the created texture
     */
    std::unique_ptr<sf::Texture> createFallbackTexture(sf::Color color, sf::Vector2u size);
    
    /**
     * Create all fallback textures when sprite sheets fail to load
     * Sets up colored rectangles for all game elements
     * 
     * @return true if fallback creation successful
     */
    bool createFallbackTextures();

public:
    // ===== Deleted Methods for Singleton Safety =====
    TextureManager(const TextureManager&) = delete;
    TextureManager& operator=(const TextureManager&) = delete;
    TextureManager(TextureManager&&) = delete;
    TextureManager& operator=(TextureManager&&) = delete;
};

#endif // TEXTURE_MANAGER_HPP