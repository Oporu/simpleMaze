
#include "../include/Game.hpp"
#include "../include/Vignette.hpp"
#include <stack>
#include <algorithm>
#include <iostream>
Game::Game(const int mazeSizeX, int mazeSizeY) :
        mazeSize(mazeSizeX, mazeSizeY),
        maze(mazeSizeY, std::vector(mazeSizeX, MazeBlock::WALL)),
		mazeWallColor(mazeSizeY, std::vector(mazeSizeX, sf::Color::White)){
    initializeWindow();
    std::uniform_int_distribution distY(1, mazeSize.y - 2);
    std::uniform_int_distribution distX(1, mazeSize.x - 2);
    const sf::Vector2i startPosition {distX(randomGen), distY(randomGen) };
    player.initializePosition(startPosition);
    initializeMaze(startPosition);
	rotation = 0.f;
	clock.restart();
	timer.restart();
}

void Game::initializeWindow() {
    window.setView({{0, 0}, static_cast<sf::Vector2f>(window.getSize())});
    window.setFramerateLimit(60);
    window.setVerticalSyncEnabled(true);
    window.setKeyRepeatEnabled(false);
    window.setActive(true);
}

void Game::initializeMaze(const sf::Vector2i& startPosition) {
    std::vector<sf::Vector2i> directions = {{0,1}, {0, -1}, {1, 0}, {-1, 0}};
    std::stack<sf::Vector2i> dfs;
    dfs.push(startPosition);

    while (!dfs.empty()) {
        const sf::Vector2i pos = dfs.top(); dfs.pop();
        if (pos.x == 0 || pos.x == mazeSize.x-1 || pos.y == 0 || pos.y == mazeSize.y-1 || maze[pos.y][pos.x] != MazeBlock::WALL) continue;
        short wallCount = 0;
        if (maze[pos.y][pos.x+1] == MazeBlock::WALL) ++wallCount;
        if (maze[pos.y][pos.x-1] == MazeBlock::WALL) ++wallCount;
        if (maze[pos.y+1][pos.x] == MazeBlock::WALL) ++wallCount;
        if (maze[pos.y-1][pos.x] == MazeBlock::WALL) ++wallCount;

        if (wallCount < 3) continue;
        maze[pos.y][pos.x] = MazeBlock::ROAD;
        std::shuffle(directions.begin(), directions.end(), randomGen);
        for (const auto &direction: directions)
            dfs.push(pos + direction);
    }

	std::uniform_int_distribution distY(1, mazeSize.y - 2);
	std::uniform_int_distribution distX(1, mazeSize.x - 2);
	sf::Vector2i exitPos {startPosition};
	while (exitPos == startPosition)
		exitPos = {distX(randomGen), distY(randomGen) };
	maze[exitPos.y][exitPos.x] = MazeBlock::EXIT;

	std::uniform_int_distribution distWall(0, 10);
	std::uniform_int_distribution<sf::Uint8> distWallAlpha(100, 230);
	for (int y = 0; y < mazeSize.y; ++y) {
		for (int x = 0; x < mazeSize.x; ++x) {
			switch (maze[y][x]) {
				case MazeBlock::WALL:
					mazeWallColor[y][x] = distWall(randomGen) ? sf::Color::White : sf::Color::Red;
					break;
				case MazeBlock::ROAD:
					mazeWallColor[y][x] = sf::Color::Transparent;
					break;
				case MazeBlock::EXIT:
					mazeWallColor[y][x] = sf::Color::Green;
					break;
			}
			mazeWallColor[y][x].a = distWallAlpha(randomGen);
		}
	}
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
    if (player.update(dt, keyPressed, maze)) {
		std::cout << timer.getElapsedTime().asSeconds() << std::endl;
        window.close();
        return;
    }
//	std::uniform_int_distribution rotate(1, 1000);x
//	std::uniform_real_distribution<float> rotateGen(0, 360);
//	rotation = rotate(randomGen) == 1 ? rotateGen(randomGen) : rotation;
	if (keyPressed[sf::Keyboard::LBracket])
		rotation -= dt*100;
	else if (keyPressed[sf::Keyboard::RBracket])
		rotation += dt*100;
}

void Game::render() {
    window.clear(sf::Color::Black);

	// render maze
    const sf::Vector2f mazeBlockSize {100, 100};
	const sf::Vector2i& playerPos = player.getPosition();
    sf::RectangleShape mazeBlockShape {mazeBlockSize};
	mazeBlockShape.setRotation(rotation);
    const sf::Vector2f offset = player.getMovingOffset();
    const int lx = std::max(0, playerPos.x - 3);
    const int rx = std::min(mazeSize.x-1, playerPos.x + 3);
    const int ly = std::max(0, playerPos.y - 3);
    const int ry = std::min(mazeSize.y-1, playerPos.y + 3);
    for (int y = ly; y <= ry; ++y) {
        for (int x = lx; x <= rx; ++x) {
//            mazeBlockShape.setPosition(mazeBlockSize.x * (static_cast<float>(x-playerPos.x) + offset.x), mazeBlockSize.y * (static_cast<float>(y-playerPos.y) + offset.y));
	        mazeBlockShape.setOrigin(-sf::Vector2f{mazeBlockSize.x * (static_cast<float>(x - playerPos.x) + offset.x),
	                                               mazeBlockSize.y * (static_cast<float>(y - playerPos.y) + offset.y)} + mazeBlockSize / 2.f);
//			mazeBlockShape.setRotation(mazeBlockShape.getRotation()+2);
			mazeBlockShape.setFillColor(mazeWallColor[y][x]);
            window.draw(mazeBlockShape);
        }
    }
	std::uniform_real_distribution dist(200.f, 215.f);
	const Vignette vignette({0, 0}, 0, dist(randomGen), 500);
	if (!keyPressed[sf::Keyboard::M]) window.draw(vignette);
    window.draw(player);

    window.display();
}
