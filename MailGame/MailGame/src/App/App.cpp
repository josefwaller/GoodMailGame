#include "App.h"
#include "Game\Game.h"
#include <SFML/Window/Event.hpp>
#include <ctime>


App::App() : window(sf::VideoMode(500, 600), "Hello World")
{
	// Initialize the window
}

void App::run() {
	// Create game
	Game game(this);
	std::clock_t lastTime = std::clock();

	while (window.isOpen()) {
		sf::Event e;
		while (window.pollEvent(e)) {
			// Poll for events
			if (e.type == sf::Event::Closed) {
				window.close();
			}
			else {
				game.onEvent(e);
			}
		}
			
		// Set up delta time and update last time
		std::clock_t nowTime = std::clock();
		float delta = (float)(nowTime - lastTime) / 1000.0f;
		lastTime = nowTime;

		// Update game
		game.update(delta);

		// Display game
		window.clear();
		game.render(&window);
		window.display();
	}
}
