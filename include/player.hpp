#ifndef SIMPLEMAZE_PLAYER_HPP
#define SIMPLEMAZE_PLAYER_HPP

#include <SFML/Graphics.hpp>
#include "MazeBlock.hpp"

class Player final : public sf::Drawable {
	inline static const float startMoveTime = 0.3f;
    sf::CircleShape shape;
    sf::Vector2i position;
    sf::Vector2i targetPosition;
    float moveTime = 0;
    bool isMoving = false;
public:
    explicit Player();
    void initializePosition(const sf::Vector2i& _position);
    bool moving() const;
    sf::Vector2f getMovingOffset() const;
    const sf::Vector2i& getPosition() const;
    bool update(float dt, const std::array<bool, sf::Keyboard::Key::KeyCount>& keyPressed, const std::vector<std::vector<MazeBlock>>& maze);
    void draw(sf::RenderTarget &target, sf::RenderStates states) const override;
};


#endif //SIMPLEMAZE_PLAYER_HPP
