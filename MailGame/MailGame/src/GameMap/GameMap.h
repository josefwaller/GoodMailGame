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
	static std::vector<sf::Sprite> ROAD_SPRITES;
	static const sf::Sprite EMPTY_SPRITE;

	GameMap(Game* g);

	Tile getTileAt(size_t x, size_t y);

	void render(sf::RenderWindow* window);

	// Set the building for a specific tile
	void setBuildingForTile(size_t x, size_t y, std::weak_ptr<Entity> building);
private:
	Game* game;
	std::vector<std::vector<Tile>> tiles;
	// Render a single tile
	void renderTile(sf::RenderWindow* window, size_t x, size_t y);
	void generateCityAt(sf::Vector2i pos);
};
