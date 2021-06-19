#pragma once
#include "GameMap/Tile/Tile.h"
#include "System/TransitType/TransitType.h"
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
	static std::vector<sf::Sprite> RAIL_TRACK_SPRITES;
	static const sf::Sprite EMPTY_SPRITE;
	// Get the corresponding ROAD_SPRITE for the road given
	static sf::Sprite getRoadSprite(Road road, IsoRotation gameRotation, std::vector<std::vector<unsigned int>> pointHeights = { {0, 0},{0,0} });
	// Create an empty game map, i.e. all tiles are empty
	GameMap(Game* g);
	// Generate a new map
	void generateNew();
	// Load a game map from the data given
	void loadFromSaveData(SaveData data);

	Tile getTileAt(size_t x, size_t y);
	Tile getTileAt(sf::Vector2i coords);
	unsigned int getPointHeight(size_t x, size_t y);
	void setPointHeight(size_t x, size_t y, unsigned int height);
	// Get the average of the point point's height around htis tile
	float getTileHeight(size_t x, size_t y);

	void render(sf::RenderWindow* window);

	// Set the building for a specific tile
	void setBuildingForTile(size_t x, size_t y, std::weak_ptr<Entity> building);
	// Set the postal code for a specific tile
	void setCodeForTile(size_t x, size_t y, id_t code);
	// Add/remove road
	void addRoad(size_t x, size_t y, Road r);
	void removeRoad(size_t x, size_t y);
	// Add rail track at the tile given
	void addRailTrack(size_t x, size_t y, IsoRotation from, IsoRotation to);
	// Add a railway station at the position given in the direction given, currently stations must be straight
	void addRailwayStation(size_t x, size_t y, IsoRotation direction);
	// Remove a railway track at the position given
	void removeRailTrack(size_t x, size_t y);
	// Add/remove airplane road
	void addAirplaneRoad(size_t x, size_t y, AirplaneRoad road);
	void removeAirplaneRoad(size_t x, size_t y);
	// Check if a vehicle of the given type can go through the tile given
	bool canGetTileLock(size_t x, size_t y, TransitType type);
	// Get a lock on a tile
	// Throws if it cannot, should check with canGetTileLock first
	void getTileLock(size_t x, size_t y, TransitType type);
	// Release a tile lock, throws if there is no lock on the tile
	void releaseTileLock(size_t x, size_t y, TransitType type);
	// Check if we can get a railway lock
	bool canGetRailwayLock(sf::Vector2i tile, Railway railway);
	// Get/release railway lock
	void getRailwayLock(sf::Vector2i tile, Railway railway);
	void releaseRailwayLock(sf::Vector2i tile, Railway railway);
	// Check if there is a road on the given tile
	bool hasRoadAt(size_t x, size_t y);
	// check if there is a road leading in the given direction on a tile
	bool hasRoadInDirection(size_t x, size_t y, IsoRotation rot);
	// Add a road in a given direction
	void addRoadInDirection(size_t x, size_t y, IsoRotation rot);
	// Get a list of all the residences on the map
	std::vector<sf::Vector2i> getResidences();
	// Get the save data for the map
	SaveData getSaveData();
private:
	Game* game;
	std::vector<std::vector<Tile>> tiles;
	std::vector<std::vector<unsigned int>> pointHeights;
	// Prepare a tile to have a road on it
	// Mostly make sure it isn't slanted weird
	void prepareTileForRoad(size_t x, size_t y, Road r);
	// Render a single tile
	void renderTile(sf::RenderWindow* window, size_t x, size_t y);
	void generateCityAt(sf::Vector2i pos, id_t cityId);
};
