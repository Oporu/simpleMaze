#ifndef SIMPLEMAZE_PLAYER_HPP
#define SIMPLEMAZE_PLAYER_HPP

#include <SFML/Graphics.hpp>
#include <array>
#include "MazeBlock.hpp"

class Player final : public sf::Drawable {
	static constexpr float startMoveTime = 0.2f;
	sf::CircleShape shape;
	sf::Vector2i position;
	sf::Vector2i targetPosition;
	float moveTime = 0;
	bool m_isMoving = false;
public:
	explicit Player();

	void setPosition(const sf::Vector2i &_position);

	sf::Vector2f getMovingOffset() const;

	const sf::Vector2i &getPosition() const;

	/*
	 * returns true if player is at exit
	 */
	bool update(float dt, const std::array<bool, sf::Keyboard::Key::KeyCount> &keyPressed,
	            const std::vector<std::vector<MazeBlock>> &maze, const sf::Vector2i &mazeExit);

	void draw(sf::RenderTarget &target, sf::RenderStates states) const override;

	bool isMoving() const;
};


#endif //SIMPLEMAZE_PLAYER_HPP
