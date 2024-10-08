#include "Game.hpp"
#include "Direction.hpp"
#include <SFML/Audio/Music.hpp>
#include <stack>
#include <algorithm>
#include <iostream>
#include <cassert>
#include <thread>

Game::Game(const int mazeSizeX, const int mazeSizeY) :
		mazeSize(mazeSizeX, mazeSizeY),
		maze(mazeSizeY, std::vector<MazeBlock>(mazeSizeX, MazeBlock{true, true})),
		mazeBlockColor(mazeSizeY, std::vector<sf::Color>(mazeSizeX, sf::Color::White)),
		window(sf::VideoMode{800, 600}, "simpleMaze " + std::to_string(mazeSizeX) + 'x' + std::to_string(mazeSizeY),
		       sf::Style::Default, sf::ContextSettings{0,0,8,1, 1}) {
	initializeWindow();
	initializeMaze();
	initializeAudioStuff();
	clock.restart();
	timer.restart();
	if (sf::Shader::isAvailable()) {
		vignetteShader.loadFromMemory("uniform float time;"
		                              "uniform vec4 color;\n"
		                              "uniform vec2 center;\n"
		                              "uniform float viewSizeWindowSizeRatio;\n"
		                              "\n"
		                              "void main()\n"
		                              "{\n"
		                              "gl_FragColor.rgb = color.rgb;\n"
		                              "gl_FragColor.a = (sin(time*40.0) + (viewSizeWindowSizeRatio * length(center - gl_FragCoord.xy))-100.0)/200.0;\n"
		                              "}", sf::Shader::Fragment);

	} else {
		std::cout << "shader not available!" << std::endl;
	}
}

void Game::initializeWindow() {
	updateView();
	window.setFramerateLimit(60);
	window.setVerticalSyncEnabled(true);
	window.setKeyRepeatEnabled(false);
}

void Game::initializeAudioStuff() {
	music.openFromFile("./bg.mp3");
	music.setVolume(3.6f);
	music.play();
	MCBuffer.loadFromFile("./walk.mp3");
	walkingSound.setBuffer(MCBuffer);
	walkingSound.setPitch(2.5f);
	walkingSound.setLoop(true);
}

void Game::initializeMaze() {
	for (std::vector<MazeBlock> &t: maze)
		std::fill(t.begin(), t.end(), MazeBlock{true, true});


	std::uniform_int_distribution distY(0, mazeSize.y - 2);
	std::uniform_int_distribution distX(0, mazeSize.x - 2);
	const sf::Vector2i startPosition{distX(randomGen), distY(randomGen)};
	player.setPosition(startPosition);
	while (mazeExit == startPosition) {
		mazeExit = {distX(randomGen), distY(randomGen)};
	}

	struct posWithDirection : sf::Vector2i {
		Direction direction; // from
	};

	std::stack<posWithDirection> dfs;
	// std::vector<posWithDirection> directions = {{{0, -1}, Direction::UP},
	//                                             {{0, 1},  Direction::DOWN},
	//                                             {{1, 0},  Direction::RIGHT},
	//                                             {{-1, 0}, Direction::LEFT}};
	std::array<posWithDirection,4> directions = {posWithDirection{{0, -1}, Direction::UP},
																							posWithDirection{{0, 1},  Direction::DOWN},
																							posWithDirection{{1, 0},  Direction::RIGHT},
																							posWithDirection{{-1, 0}, Direction::LEFT}};
	std::shuffle(directions.begin(), directions.end(), randomGen);
	for (const auto &direction: directions)
		dfs.push({startPosition + direction, direction.direction});

	while (!dfs.empty()) {
		posWithDirection pos = dfs.top();
		dfs.pop();
		if ((pos.x < 0 || pos.x >= mazeSize.x - 1 || pos.y < 0 || pos.y >= mazeSize.y - 1)
				|| (!(maze[pos.y][pos.x].left && maze[pos.y][pos.x].top && maze[pos.y][pos.x + 1].left &&
							maze[pos.y + 1][pos.x].top))) {
			continue;
		}
		switch (pos.direction) {
			case Direction::UP:
				maze[pos.y + 1][pos.x].top = false;
				break;
			case Direction::DOWN:
				maze[pos.y][pos.x].top = false;
				break;
			case Direction::LEFT:
				maze[pos.y][pos.x + 1].left = false;
				break;
			case Direction::RIGHT:
				maze[pos.y][pos.x].left = false;
				break;
			default:
				break;
		}
		if (pos == mazeExit) continue;
		std::shuffle(directions.begin(), directions.end(), randomGen);
		for (const auto &direction: directions)
			dfs.push({pos + direction, direction.direction});
	}
	std::uniform_int_distribution distBlockAlpha(70, 230);
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
			case Event::Resized:
				updateView();
				break;
			default:
				break;
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
		std::cout << "simpleMaze " << mazeSize.x << 'x' << mazeSize.y << " completed in "
		          << timer.getElapsedTime().asSeconds() << " seconds" << std::endl;

		walkingSound.stop();
		music.openFromFile("./win.mp3");
		music.play();
		for (int i = 0; i < 10; ++i) {
			sf::RenderWindow windowa(sf::VideoMode{700, 10}, "winnnn11!1");
			std::this_thread::sleep_for(std::chrono::milliseconds(250));
		}
		window.close();

		return;
	}
	if (player.isMoving()) {
		if (walkingSound.getStatus() == sf::SoundSource::Paused ||
		    walkingSound.getStatus() == sf::SoundSource::Stopped) {
			walkingSound.play();
		}
	} else if (walkingSound.getStatus() == sf::SoundSource::Playing) {
		walkingSound.pause();
	}
}

void Game::render() {
	window.clear(sf::Color::Black);

	// render maze
	const sf::Vector2f mazeBlockSize{100, 100};
	const sf::Vector2i &playerPos = player.getPosition();
	sf::RectangleShape mazeBlockShape{mazeBlockSize};
	const sf::Vector2f offset = player.getMovingOffset();
	const int lx = std::max(0, playerPos.x - 4);
	const int rx = std::min(mazeSize.x - 1, playerPos.x + 4);
	const int ly = std::max(0, playerPos.y - 4);
	const int ry = std::min(mazeSize.y - 1, playerPos.y + 4);

	// render floor
	for (int y = ly; y <= ry; ++y) {
		for (int x = lx; x <= rx; ++x) {
			mazeBlockShape.setOrigin(-sf::Vector2f{mazeBlockSize.x * (static_cast<float>(x - playerPos.x) + offset.x),
			                                       mazeBlockSize.y * (static_cast<float>(y - playerPos.y) + offset.y)} +
			                         mazeBlockSize / 2.f);
			mazeBlockShape.setFillColor(mazeBlockColor[y][x]);
			window.draw(mazeBlockShape);
		}
	}

	// render shadow of wall
	for (int y = ly; y <= ry; ++y) {
		for (int x = lx; x <= rx; ++x) {
			renderMazeBlockShadows(x, y, offset, mazeBlockSize, mazeBlockShape);
		}
	}
	window.draw(player);

	sf::RectangleShape vignette(window.getView().getSize());
	vignette.setPosition(-window.getView().getSize() / 2.f);
	vignette.setFillColor(sf::Color::Black);
	float viewSizeWindowSizeRatio = window.getView().getSize().x / static_cast<float>(window.getSize().x);
	vignetteShader.setUniform("time", timer.getElapsedTime().asSeconds());
	vignetteShader.setUniform("center", static_cast<sf::Vector2f>(window.getSize()) / 2.f);
	vignetteShader.setUniform("viewSizeWindowSizeRatio", viewSizeWindowSizeRatio);
	if (!keyPressed[sf::Keyboard::M]) window.draw(vignette, &vignetteShader);


	window.display();
}

void Game::renderShadowByFace(sf::Vector2f a, sf::Vector2f b, sf::Color color) {
	sf::Vector2f c = b * 10.f;
	sf::Vector2f d = a * 10.f;
	sf::ConvexShape s(4);
	s.setFillColor(color);
	s.setPoint(0, a);
	s.setPoint(1, b);
	s.setPoint(2, c);
	s.setPoint(3, d);
	window.draw(s);
}

void
Game::renderMazeBlockShadows(const int x, const int y, const sf::Vector2f &offset, const sf::Vector2f &mazeBlockSize,
                             const sf::RectangleShape &mazeBlockShape) {
	if (x < 0 || y < 0 || x >= mazeSize.x || y >= mazeSize.y) return;
	if (!(maze[y][x].top || maze[y][x].left)) return;
	const sf::Vector2i &playerPos = player.getPosition();
	const sf::Vector2f p = sf::Vector2f{mazeBlockSize.x * (static_cast<float>(x - playerPos.x) + offset.x),
	                                    mazeBlockSize.y * (static_cast<float>(y - playerPos.y) + offset.y)} -
	                       mazeBlockSize / 2.f;// + sf::Vector2f{.5f, .5f};
	if (maze[y][x].top)
		renderShadowByFace(p + mazeBlockShape.getPoint(0), p + mazeBlockShape.getPoint(1), sf::Color::Black);
	if (maze[y][x].left)
		renderShadowByFace(p + mazeBlockShape.getPoint(0), p + mazeBlockShape.getPoint(3), sf::Color::Black);
}

void Game::updateView() {
	sf::Vector2f windowSize = static_cast<sf::Vector2f>(window.getSize());
	float windowSizeRatio = windowSize.x / windowSize.y;
	sf::Vector2f viewSize;
	if (windowSize.y * viewSizeRatio > windowSize.x) {
		viewSize.x = 600 * windowSizeRatio;
		viewSize.y = 600;
	} else {
		viewSize.x = 800;
		viewSize.y = 800 / windowSizeRatio;
	}
	window.setView({{0, 0}, viewSize});
}
