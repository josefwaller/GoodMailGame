#include "App.h"
#include "Game\Game.h"
#include "Constants.h"
#include <SFML/Window/Event.hpp>
#include <SFML/System/Clock.hpp>
#include <imgui-SFML.h>
#include <imgui.h>


App::App() : window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Hello World"), lastTime()
{
	// Initialize the window
}

void App::run() {
	// Create game
	Game game(this, &this->window);
	sf::Clock deltaClock;
	ImGui::SFML::Init(window);

	while (window.isOpen()) {
		sf::Event e;
		while (window.pollEvent(e)) {
			ImGui::SFML::ProcessEvent(e);
			// Poll for events
			if (e.type == sf::Event::Closed) {
				window.close();
			}
			else {
				game.onEvent(e);
			}
		}
			
		// Set up delta time and update last time
		sf::Time nowTime = deltaClock.restart();
		float delta = nowTime.asMilliseconds() / 1000.0f;

		// Update ImGui
		ImGui::SFML::Update(window, nowTime);

		// Update game
		game.update(delta);

		// Display game
		window.clear();
		game.render(&window);
		ImGui::SFML::Render(window);
		window.display();
	}
	ImGui::SFML::Shutdown();
}
