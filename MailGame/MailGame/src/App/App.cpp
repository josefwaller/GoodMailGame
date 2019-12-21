#include "App.h"
#include <SFML/Window/Event.hpp>



App::App() : window(sf::VideoMode(500, 600), "Hello World")
{
	// Initialize the window
}

void App::run() {
	while (window.isOpen()) {
		sf::Event e;
		while (window.pollEvent(e)) {
			if (e.type == sf::Event::Closed) {
				window.close();
			}

			window.clear();
			window.display();
		}
	}
}
