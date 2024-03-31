#include "../include/player.hpp"

Player::Player() : shape(30) {
	shape.setPosition(0, 0);
	shape.setOrigin(shape.getRadius(), shape.getRadius());
	shape.setFillColor(sf::Color::Blue);
}

void Player::draw(sf::RenderTarget &target, sf::RenderStates states) const {
	target.draw(shape, states);
}

void Player::initializePosition(const sf::Vector2i &_position) {
	position = _position;
}

bool Player::moving() const {
	return isMoving;
}

bool Player::update(const float dt, const std::array<bool, sf::Keyboard::Key::KeyCount>& keyPressed, const std::vector<std::vector<MazeBlock>>& maze) {
	if (!isMoving) {
		targetPosition = position;
		if ((keyPressed[sf::Keyboard::W] || keyPressed[sf::Keyboard::Up]) && maze[position.y-1][position.x] != MazeBlock::WALL) {
			--targetPosition.y;
			isMoving = true;
		} else if ((keyPressed[sf::Keyboard::A] || keyPressed[sf::Keyboard::Left]) && maze[position.y][position.x-1] != MazeBlock::WALL) {
			--targetPosition.x;
			isMoving = true;
		} else if ((keyPressed[sf::Keyboard::S] || keyPressed[sf::Keyboard::Down]) && maze[position.y+1][position.x] != MazeBlock::WALL) {
			++targetPosition.y;
			isMoving = true;
		} else if ((keyPressed[sf::Keyboard::D] || keyPressed[sf::Keyboard::Right]) && maze[position.y][position.x+1] != MazeBlock::WALL) {
			++targetPosition.x;
			isMoving = true;
		}
		if (isMoving) {
			moveTime = 0.3f;
		}
	}
	if (isMoving) {
		// moveTime - dt <= 0
		if (moveTime <= dt) {
			isMoving = false;
			position = targetPosition;
			if (maze[position.y][position.x] == MazeBlock::EXIT) {
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
