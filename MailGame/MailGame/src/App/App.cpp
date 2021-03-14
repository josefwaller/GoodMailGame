#include "App.h"
#include "Game\Game.h"
#include "Constants.h"
#include <SFML/Window/Event.hpp>
#include <SFML/System/Clock.hpp>
#include <imgui-SFML.h>
#include <imgui.h>
#include <rapidxml.hpp>
#include <fstream>
#include <stdio.h>
#include "System/SaveData/SaveData.h"

App::App() : window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Hello World"), lastTime(), game(this, &this->window), menu(this), inMainMenu(true)
{
	// Initialize the window
}

void App::generateNewGame() {
	game.generateNew();
	this->inMainMenu = false;
}

void App::loadSaveFile(std::string savePath) {
	// Load test save data
	std::ifstream fs("savedata/" + savePath + ".xml");
	std::string data((std::istreambuf_iterator<char>(fs)), (std::istreambuf_iterator<char>()));
	SaveData sd = SaveData::fromFileContents(data);
	game.loadFromSaveData(sd);
	game.getUi()->setSavefileName(savePath.c_str());
	this->inMainMenu = false;
}

void App::run() {
	sf::Clock deltaClock;
	ImGui::SFML::Init(window);

	while (window.isOpen()) {
		sf::Event e;
		while (window.pollEvent(e)) {
			ImGui::SFML::ProcessEvent(e);
			// Poll for events
			if (e.type == sf::Event::Closed) {
				window.close();
				break;
			}
			else {
				if (!this->inMainMenu)
					this->game.onEvent(e);
			}
		}

		// Set up delta time and update last time
		sf::Time nowTime = deltaClock.restart();
		float delta = nowTime.asMilliseconds() / 1000.0f;

		// Update ImGui
		ImGui::SFML::Update(window, nowTime);

		// Update game
		if (this->inMainMenu) {
			this->menu.update();
		}
		else {
			this->game.update(delta);
		}

		// Display game
		window.clear();
		if (!this->inMainMenu)
			this->game.render(&window);
		ImGui::SFML::Render(window);
		window.display();
	}
	ImGui::SFML::Shutdown();
}