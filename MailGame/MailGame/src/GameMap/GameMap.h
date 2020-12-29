#pragma once
#include "GameMap/Tile.h"
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <vector>
class Game;
class SaveData;

class GameMap {
public:
	// The dimensions of the map
	// Will eventually just be a preset size and allow the player to have a custom size
	static const size_t MAP_WIDTH;
	static const size_t MAP_HEIGHT;

	// The tiles used to render the map
	static std::vector<sf::Sprite> ROAD_SPRITES;
	static std::vector<sf::Sprite> RAIL_TRACK_SPRITES;
	static const sf::Sprite EMPTY_SPRITE;
	// Create an empty game map, i.e. all tiles are empty
	GameMap(Game* g);
	// Generate a new map
	void generateNew();
	// Load a game map from the data given
	void loadFromSaveData(SaveData data);

	Tile getTileAt(size_t x, size_t y);

	void render(sf::RenderWindow* window);

	// Set the building for a specific tile
	void setBuildingForTile(size_t x, size_t y, std::weak_ptr<Entity> building);
	// Set the postal code for a specific tile
	void setCodeForTile(size_t x, size_t y, long long code);
	// Add rail track at the tile given
	void addRailTrack(size_t x, size_t y, IsoRotation from, IsoRotation to, hour_t hour = 0);
	// Remove a railway track at the position given
	void removeRailTrack(size_t x, size_t y, hour_t hour);
	// Check if there is a road on the given tile
	bool hasRoadAt(size_t x, size_t y);
	// check if there is a road leading in the given direction on a tile
	bool hasRoadInDirection(size_t x, size_t y, IsoRotation rot);
	// Add a road in a given direction
	void addRoadInDirection(size_t x, size_t y, IsoRotation rot);
	// Get the save data for the map
	SaveData getSaveData();
private:
	Game* game;
	std::vector<std::vector<Tile>> tiles;
	// Initialize the tile sprites
	void initTileSprites();
	// Render a single tile
	void renderTile(sf::RenderWindow* window, size_t x, size_t y);
	void generateCityAt(sf::Vector2i pos);
};
