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
	lastTime = std::time(nullptr);

	while (window.isOpen()) {
		sf::Event e;
		while (window.pollEvent(e)) {
			// Set up delta time and update last time
			float nowTime = std::time(nullptr);
			float delta = lastTime - nowTime;
			lastTime = nowTime;

			// Poll for events
			if (e.type == sf::Event::Closed) {
				window.close();
			}
			
			// Update game
			game.update(delta);

			// Display game
			window.clear();
			game.render(&window);
			window.display();
		}
	}
}
