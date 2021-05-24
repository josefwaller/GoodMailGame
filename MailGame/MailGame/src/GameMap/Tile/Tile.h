#pragma once
#include <memory>
#include <optional>
#include <map>
#include "System/IsoRotation/IsoRotation.h"
#include "Railway/Railway.h"
#include "Constants.h"
#include "./Road/Road.h"
#include "./Road/AirplaneRoad/AirplaneRoad.h"

// Forward declaration
class Entity;

// The three types of tiles
enum class TileType {
	Land,
	Water,
	OffMap
};

struct Tile {
	// The type of tile
	TileType type;
	// The building on the tile
	std::weak_ptr<Entity> building;
	// The postal code assigned to the tile
	id_t postalCode;
	// The road, if it has one
	std::optional<Road> road;
	// The road for airplanes, if it has one
	std::optional<AirplaneRoad> airplaneRoad;
	// The tile lock, currently just one for all transit types
	bool isLocked;
	// The id of the city the tile belongs to (0 is no city)
	id_t cityId;

	Tile(TileType t = TileType::Land, id_t cityId = 0);
	/**
	 *Get the connected tiles from the ingoing direction provided
	 * ingoingDirection is the direction the train/etc is coming from, relative to the tile
	 * i.e. if the ingoinDirection is south, the train is coming from the tile below this one
	 */
	std::vector<IsoRotation> getOutgoingRailDirections(IsoRotation ingoingDirection);
	// Get the railways
	std::vector<Railway> getRailways();
	// Remove all railways
	void deleteRailways();
	// Add a railway to this tile
	void addRailway(Railway r);

	// Check whether it can get a tile lock
	bool canGetLock();

	/*** These methods ignore everything from the railway object except from and to directions ***/
	// Check if we can get a railway lock on this tile
	bool canGetRailwayLock(Railway r);
	// Get/release a railway lock
	void getRailwayLock(Railway r);
	void releaseRailwayLock(Railway r);
private:
	// The railways on this tile
	std::vector<Railway> railways;
};
