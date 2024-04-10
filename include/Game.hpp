#ifndef GAME_HPP
#define GAME_HPP

#include <SFML/Graphics.hpp>
#include "player.hpp"
#include "MazeBlock.hpp"
#include "Direction.hpp"
#include <random>
#include <array>
class Game final {
	std::random_device randomDevice;
	std::mt19937 randomGen{randomDevice()};
	sf::RenderWindow window { sf::VideoMode(800, 600), "simpleMaze", sf::Style::Close };
	sf::Clock clock;
	sf::Clock timer;
	Player player;
	const sf::Vector2i mazeSize;
	std::vector<std::vector<MazeBlock>> maze;
	sf::Vector2i mazeExit;
	std::vector<std::vector<sf::Color>> mazeBlockColor;
	std::array<bool, sf::Keyboard::Key::KeyCount> keyPressed {false};
	float rotation;
	void initializeMaze();
	void initializeWindow();
	void handleWindowEvents();
	void renderMazeBlock(int x, int y, const sf::Vector2f& offset, const sf::Vector2f& mazeBlockSize, const sf::RectangleShape& mazeBlockShape);
	void renderShadowByFace(sf::Vector2f a, sf::Vector2f b, sf::Color color=sf::Color(255,255,255,255));
public:
	explicit Game(int mazeSizeX = 10, int mazeSizeY = 10);
	bool isActive() const;
	void update();
	void render();

};

#endif