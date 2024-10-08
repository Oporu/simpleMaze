#include "player.hpp"

Player::Player() : shape(30) {
	shape.setPosition(0, 0);
	shape.setOrigin(shape.getRadius(), shape.getRadius());
	shape.setFillColor(sf::Color::Blue);
}

void Player::draw(sf::RenderTarget &target, sf::RenderStates states) const {
	target.draw(shape, states);
}

bool Player::isMoving() const {
	return m_isMoving;
}

void Player::setPosition(const sf::Vector2i &_position) {
	position = _position;
}

bool Player::update(const float dt, const std::array<bool, sf::Keyboard::Key::KeyCount> &keyPressed,
                    const std::vector<std::vector<MazeBlock>> &maze, const sf::Vector2i &mazeExit) {
	if (!m_isMoving) {
		targetPosition = position;
		if ((keyPressed[sf::Keyboard::W] || keyPressed[sf::Keyboard::Up]) && !maze[position.y][position.x].top) {
			--targetPosition.y;
			m_isMoving = true;
		} else if ((keyPressed[sf::Keyboard::A] || keyPressed[sf::Keyboard::Left]) &&
		           !maze[position.y][position.x].left) {
			--targetPosition.x;
			m_isMoving = true;
		} else if ((keyPressed[sf::Keyboard::S] || keyPressed[sf::Keyboard::Down]) &&
		           !maze[position.y + 1][position.x].top) {
			++targetPosition.y;
			m_isMoving = true;
		} else if ((keyPressed[sf::Keyboard::D] || keyPressed[sf::Keyboard::Right]) &&
		           !maze[position.y][position.x + 1].left) {
			++targetPosition.x;
			m_isMoving = true;
		}
		if (m_isMoving) {
			moveTime = startMoveTime;
		}
	}
	if (m_isMoving) {
		// moveTime - dt <= 0
		if (moveTime <= dt) {
			m_isMoving = false;
			position = targetPosition;
			if (position == mazeExit) {
				return true;
			}
		}
		moveTime -= dt;
	}
	return false;
}

sf::Vector2f Player::getMovingOffset() const {
	return static_cast<sf::Vector2f>(position - targetPosition) * ((startMoveTime - moveTime) / startMoveTime);
}

const sf::Vector2i &Player::getPosition() const {
	return position;
}
