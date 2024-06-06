#ifndef GAME_HPP
#define GAME_HPP

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <random>
#include <array>
#include "player.hpp"
#include "MazeBlock.hpp"
#include "Direction.hpp"


class Game final {
	std::mt19937 randomGen{std::random_device()()};
	sf::RenderWindow window;
	sf::Clock clock;
	sf::Clock timer;
	Player player;
	const sf::Vector2i mazeSize;
	std::vector<std::vector<MazeBlock>> maze;
	sf::Vector2i mazeExit;
	std::vector<std::vector<sf::Color>> mazeBlockColor;
	sf::Shader vignetteShader;
	std::array<bool, sf::Keyboard::Key::KeyCount> keyPressed{false};
	sf::Sound walkingSound;
	sf::Music music;
	sf::SoundBuffer MCBuffer;

	void initializeAudioStuff();

	void initializeMaze();

	void initializeWindow();

	void handleWindowEvents();

	void renderMazeBlockShadows(int x, int y, const sf::Vector2f &offset, const sf::Vector2f &mazeBlockSize,
	                            const sf::RectangleShape &mazeBlockShape);

	void renderShadowByFace(sf::Vector2f a, sf::Vector2f b, sf::Color color = sf::Color(255, 255, 255, 255));

public:
	explicit Game(int mazeSizeX = 10, int mazeSizeY = 10);

	bool isActive() const;

	void update();

	void render();

};

#endif