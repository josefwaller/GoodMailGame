#pragma once
#include "GameMap/Tile/Tile.h"
#include "./Tunnel/Tunnel.h"
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

	// Min and max point height
	static const unsigned int MIN_HEIGHT;
	static const unsigned int MAX_HEIGHT;

	// The sea level
	// Any tiles below this are underwater
	static const unsigned int SEA_LEVEL;

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
	unsigned int getPointHeight(sf::Vector2i point);
	void setPointHeight(size_t x, size_t y, unsigned int height);
	// Get the average of the point point's height around htis tile
	float getTileHeight(size_t x, size_t y);
	// Get the height of the lowest point on this tile
	float getLowestTileHeight(size_t x, size_t y);
	float getHeightAt(float x, float y);
	float getHeightAt(sf::Vector2f);

	void render(sf::RenderWindow* window);
	void renderRailway(sf::Vector2i tile, Railway rail, sf::RenderWindow* window);

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
	// Add a railway signal to the tile
	void addRailwaySignal(sf::Vector2i tile);
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
	// Get/release tunnel locks
	bool canGetTunnelLock(Tunnel t);
	void getTunnelLock(Tunnel t);
	void releaseTunnelLock(Tunnel t);
	// Check if there is a road on the given tile
	bool hasRoadAt(size_t x, size_t y);
	// check if there is a road leading in the given direction on a tile
	bool hasRoadInDirection(size_t x, size_t y, IsoRotation rot);
	// Add a road in a given direction
	void addRoadInDirection(size_t x, size_t y, IsoRotation rot);
	// Add a tunnel
	void addTunnel(sf::Vector2i start, sf::Vector2i end, TransitType type);
	// Remove tunnel
	void removeTunnel(size_t x, size_t y);
	// Get the tunnels that start or end at a given tile
	std::vector<Tunnel> getTunnelsForTile(sf::Vector2i tile);
	// Get tunnels that start of end on a tile, not accounding for direction
	std::vector<Tunnel> getTunnels(sf::Vector2i tile);
	// Get a list of all the residences on the map
	std::vector<sf::Vector2i> getResidences();
	// Get the height of the higher point of the two in the direction given
	// I.e. if rot is North, then get the higher hight of the top two points
	unsigned int getMaxHeightInDirection(size_t x, size_t y, IsoRotation rot);
	// Get the save data for the map
	SaveData getSaveData();
private:
	Game* game;
	std::vector<std::vector<Tile>> tiles;
	std::vector<std::vector<unsigned int>> pointHeights;
	// Whether a point height is locked, i.e. cannot be changed
	// Usually when a road or building is on it
	std::vector<std::vector<bool>> pointHeightLocked;
	// Prepare a tile to have a road on it
	// Mostly make sure it isn't slanted weird
	void prepareTileForRoad(size_t x, size_t y, Road r);
	// Render a single tile
	void renderTile(sf::RenderWindow* window, size_t x, size_t y);
	void generateCityAt(sf::Vector2i pos, id_t cityId);
	std::vector<sf::Vector2i> residenceLocations;
	std::vector<Tunnel> tunnels;
};
