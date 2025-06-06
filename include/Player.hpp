#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <SFML/Graphics.hpp>

class Player {
private:
    sf::RectangleShape shape; // Player shape
    double posX;
    double posY;             // Player position
    double velocityY;         // Vertical velocity for jumping
    bool isJumping;           // Jumping status

    static const double GROUND_Y; // Ground level Y coordinate
    static const double JUMP_STRENGTH; // Initial jump velocity
    static const double GRAVITY; // Gravity acceleration

public:
    Player(float startX, float startY);   
    ~Player();

    // 행동 메서드들
    void jump();                    // do jump
    void update(float deltaTime);   // physics update
    void render(sf::RenderWindow& window); // render player on screen
    void reset();                   // reset player position and state

    // 정보 제공 메서드들 (const: 데이터를 변경하지 않음을 보장)
    const sf::RectangleShape& getShape() const;  // check player shape
    bool getIsJumping() const;      // check if player is jumping
    double getPosX() const;          // check x position
    double getPosY() const;          // check y position
};


#endif