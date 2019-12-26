#pragma once
#include "GameMap/Tile.h"
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <vector>
class Game;

class GameMap {
public:
	// The dimensions of the map
	// Will eventually just be a preset size and allow the player to have a custom size
	static const size_t MAP_WIDTH;
	static const size_t MAP_HEIGHT;

	// The tiles used to render the map
	static sf::Sprite ROAD_SPRITE;
	static sf::Sprite EMPTY_SPRITE;

	GameMap(Game* g);

	Tile getTileAt(size_t x, size_t y);

	void render(sf::RenderWindow* window);
private:
	void generateCityAt(sf::Vector2i pos);
	std::vector<std::vector<Tile>> tiles;
};
