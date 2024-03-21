#ifndef GAME_HPP
#define GAME_HPP

#include <SFML/Graphics.hpp>
#include "player.hpp"
#include "MazeBlock.hpp"
#include <random>
class Game final {
	std::random_device randomDevice;
	std::mt19937 randomGen{randomDevice()};
	sf::RenderWindow window { sf::VideoMode(800, 600), "simpleMaze", sf::Style::Close };
	sf::Clock clock;
	sf::Clock timer;
    Player player;
    const sf::Vector2i mazeSize;
	std::vector<std::vector<MazeBlock>> maze;
	std::vector<std::vector<sf::Color>> mazeWallColor;
    std::array<bool, sf::Keyboard::Key::KeyCount> keyPressed {false};
	float rotation;
    void initializeMaze(const sf::Vector2i& startPosition);
    void initializeWindow();
	void handleWindowEvents();
public:
	explicit Game(int mazeSizeX = 10, int mazeSizeY = 10);
	bool isActive() const;
	void update();
	void render();
};

#endif