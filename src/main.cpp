#include "Game.hpp"
#include <iostream>

int main(int argc, char *argv[]) {
	int mazeSizeX = 10, mazeSizeY = 10;
	if (argc == 3) {
		try {
			mazeSizeX = std::stoi(argv[1]);
			mazeSizeY = std::stoi(argv[2]);
		} catch (const std::invalid_argument &e) {
			std::cout << "invalid args" << std::endl;
			return 1;
		} catch (const std::out_of_range &e) {
			std::cout << "invalid args" << std::endl;
			return 1;
		}
		if (mazeSizeX < 4 || mazeSizeY < 4) {
			std::cout << "invalid args" << std::endl;
			return 1;
		}
	}
	Game game(mazeSizeX, mazeSizeY);
	while (game.isActive()) {
		game.update();
		game.render();
	}
}
