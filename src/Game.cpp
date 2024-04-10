
#include "../include/Game.hpp"
#include "../include/Vignette.hpp"
#include "../include/Direction.hpp"
#include <stack>
#include <algorithm>
#include <iostream>
Game::Game(const int mazeSizeX, int mazeSizeY) :
		mazeSize(mazeSizeX, mazeSizeY),
		maze(mazeSizeY, std::vector(mazeSizeX, MazeBlock{true, true})),
		mazeBlockColor(mazeSizeY, std::vector(mazeSizeX, sf::Color::White)){
	initializeWindow();
//	std::uniform_int_distribution distY(1, mazeSize.y - 2);
//	std::uniform_int_distribution distX(1, mazeSize.x - 2);
//	const sf::Vector2i startPosition {distX(randomGen), distY(randomGen) };
//	player.setPosition(startPosition);
	initializeMaze();
	rotation = 0.f;
	clock.restart();
	timer.restart();
}

void Game::initializeWindow() {
	window.setView({{0, 0}, static_cast<sf::Vector2f>(window.getSize())});
//	window.setFramerateLimit(60);
	window.setVerticalSyncEnabled(true);
	window.setKeyRepeatEnabled(false);
	window.setActive(true);
}

void Game::initializeMaze() {
	struct tempaaaa : sf::Vector2i {
		Direction direction;
	};
	std::uniform_int_distribution distY(0, mazeSize.y - 2);
	std::uniform_int_distribution distX(0, mazeSize.x - 2);
	const sf::Vector2i startPosition {distX(randomGen), distY(randomGen)};
	player.setPosition(startPosition);
	std::vector<tempaaaa> directions = {{{0, -1}, Direction::UP},
	                                    {{0, 1}, Direction::DOWN},
	                                    {{1, 0}, Direction::RIGHT},
	                                    {{-1, 0}, Direction::LEFT}};
	std::stack<tempaaaa> dfs;
	{
		std::shuffle(directions.begin(), directions.end(), randomGen);
		for (const auto &direction: directions)
			dfs.push({startPosition + direction, direction.direction});
	}
	while (!dfs.empty()) {
		const tempaaaa pos = dfs.top(); dfs.pop();
		if (pos.x < 0 || pos.x >= mazeSize.x-1 || pos.y < 0 || pos.y >= mazeSize.y-1) continue;
		if (!(maze[pos.y][pos.x].left && maze[pos.y][pos.x].top && maze[pos.y][pos.x+1].left && maze[pos.y+1][pos.x].top)) continue;
		switch (pos.direction) {
			case Direction::UP:
				maze[pos.y+1][pos.x].top = false;
				break;
			case Direction::DOWN:
				maze[pos.y][pos.x].top = false;
				break;
			case Direction::LEFT:
				maze[pos.y][pos.x+1].left = false;
				break;
			case Direction::RIGHT:
				maze[pos.y][pos.x].left = false;
				break;
			default:;
		}
		std::shuffle(directions.begin(), directions.end(), randomGen);
		for (const auto &direction: directions)
			dfs.push({pos + direction, direction.direction});
	}

	do {
		mazeExit = {distX(randomGen), distY(randomGen) };
	} while (startPosition == mazeExit);
	std::uniform_int_distribution distBlockAlpha(70, 230);
	std::uniform_int_distribution distWall(0, 10);
	for (int y = 0; y < mazeSize.y; ++y) {
		for (int x = 0; x < mazeSize.x; ++x) {
			mazeBlockColor[y][x] = sf::Color::White;
			mazeBlockColor[y][x].a = distBlockAlpha(randomGen);
		}
	}
	mazeBlockColor[mazeExit.y][mazeExit.x] = sf::Color::Green;
}

void Game::handleWindowEvents() {
	sf::Event event{};
	while (window.pollEvent(event)) {
		switch (event.type) {
			using sf::Event;
			case Event::Closed:
				window.close();
				return;
			case Event::KeyPressed:
				keyPressed[event.key.code] = true;
				if (event.key.code == sf::Keyboard::Escape) {
					window.close();
					return;
				}
				break;
			case Event::KeyReleased:
				keyPressed[event.key.code] = false;
				break;
			default:;
		}
	}
}

bool Game::isActive() const {
	return window.isOpen();
}

void Game::update() {
	const float dt = clock.restart().asSeconds();
	handleWindowEvents();
	if (player.update(dt, keyPressed, maze, mazeExit)) {
		std::cout << timer.getElapsedTime().asSeconds() << std::endl;
		window.close();
		return;
	}
//	std::uniform_int_distribution rotate(1, 1000);x
//	std::uniform_real_distribution<float> rotateGen(0, 360);
//	rotation = rotate(randomGen) == 1 ? rotateGen(randomGen) : rotation;

//	if (keyPressed[sf::Keyboard::LBracket])
//		rotation -= dt*100;
//	else if (keyPressed[sf::Keyboard::RBracket])
//		rotation += dt*100;
}

void Game::render() {
	window.clear(sf::Color::Black);

	// render maze
	const sf::Vector2f mazeBlockSize {100, 100};
	const sf::Vector2i& playerPos = player.getPosition();
	sf::RectangleShape mazeBlockShape {mazeBlockSize};
	mazeBlockShape.setRotation(rotation);
	const sf::Vector2f offset = player.getMovingOffset();
	const int lx = std::max(0, playerPos.x - 4);
	const int rx = std::min(mazeSize.x-1, playerPos.x + 4);
	const int ly = std::max(0, playerPos.y - 4);
	const int ry = std::min(mazeSize.y-1, playerPos.y + 4);
	for (int y = ly; y <= ry; ++y) {
		for (int x = lx; x <= rx; ++x) {
			mazeBlockShape.setOrigin(-sf::Vector2f{mazeBlockSize.x * (static_cast<float>(x - playerPos.x) + offset.x),
												   mazeBlockSize.y * (static_cast<float>(y - playerPos.y) + offset.y)} + mazeBlockSize / 2.f);
			mazeBlockShape.setFillColor(mazeBlockColor[y][x]);
			window.draw(mazeBlockShape);
		}
	}

	for (int y = ly; y <= ry; ++y) {
		for (int x = lx; x <= rx; ++x) {
			renderMazeBlock(x, y, offset, mazeBlockSize, mazeBlockShape);
		}
	}
	std::uniform_real_distribution dist(440.f, 450.f);
	const Vignette vignette({0, 0}, 0, dist(randomGen), 500);
//	const Vignette vignette({0, 0}, 0, 400, 500);
	if (!keyPressed[sf::Keyboard::M]) window.draw(vignette);
	window.draw(player);

	window.display();
}

void Game::renderShadowByFace(sf::Vector2f a, sf::Vector2f b, sf::Color color) {
	//a d
	//b c
	sf::Vector2f c = b*10.f;
	sf::Vector2f d = a*10.f;
	sf::ConvexShape s(4);
//	s.setFillColor(sf::Color(255,255,0,100));
	s.setFillColor(color);
	s.setPoint(0, a);
	s.setPoint(1, b);
	s.setPoint(2, c);
	s.setPoint(3, d);
//	sf::Vertex v[] = {{a, sf::Color::Cyan}, {b,sf::Color::Cyan}};
//	window.draw(v, 2, sf::Lines);
	window.draw(s);
}

void Game::renderMazeBlock(const int x, const int y, const sf::Vector2f& offset, const sf::Vector2f& mazeBlockSize, const sf::RectangleShape& mazeBlockShape) {
	if (x < 0 || y < 0 || x >= mazeSize.x || y >= mazeSize.y) return;
	if (!(maze[y][x].top || maze[y][x].left)) return;
	const sf::Vector2i& playerPos = player.getPosition();
	const sf::Vector2f p = sf::Vector2f{mazeBlockSize.x * (static_cast<float>(x - playerPos.x) + offset.x),
	                                    mazeBlockSize.y * (static_cast<float>(y - playerPos.y) + offset.y)} - mazeBlockSize / 2.f + sf::Vector2f{.5f, .5f};
	if (maze[y][x].top) renderShadowByFace(p+mazeBlockShape.getPoint(0), p+mazeBlockShape.getPoint(1), sf::Color::Black);
	if (maze[y][x].left) renderShadowByFace(p+mazeBlockShape.getPoint(0), p+mazeBlockShape.getPoint(3), sf::Color::Black);
}
