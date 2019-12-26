#pragma once
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/View.hpp>
#include "GameMap/GameMap.h"

class App;

class Game {
public:
	Game(App * a);
	void update(float delta);
	void onEvent(sf::Event e);
	void render(sf::RenderWindow* window);
private:
	GameMap gameMap;
	// The view of the game
	// Includes camera position
	sf::View gameView;
};
