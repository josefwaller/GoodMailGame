#pragma once
#include <SFML/Graphics/RenderWindow.hpp>
#include "GameMap/GameMap.h"

class App;

class Game {
public:
	Game(App * a);
	void update(float delta);
	void render(sf::RenderWindow* window);
private:
	GameMap gameMap;
};
