#pragma once
#include "GameMap/Tile.h"
#include <SFML/Graphics/RenderWindow.hpp>
#include <vector>
class Game;

class GameMap {
public:
	static const size_t MAP_WIDTH;
	static const size_t MAP_HEIGHT;
	GameMap(Game* g);

	Tile getTileAt(size_t x, size_t y);

	void render(sf::RenderWindow* window);
private:
	std::vector<std::vector<Tile>> tiles;
};
