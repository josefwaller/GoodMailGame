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
	// The railway, if it has one
	// The railway
	std::optional<Railway> railway;
	// The road, if it has one
	std::optional<Road> road;
	// The road for airplanes, if it has one
	std::optional<AirplaneRoad> airplaneRoad;
	// The tile lock, currently just one for all transit types
	bool isLocked;
	// The id of the city the tile belongs to (0 is no city)
	id_t cityId;

	Tile(TileType t = TileType::Land, id_t cityId = 0) : postalCode(0), isLocked(false), cityId(cityId) {
		type = t;
	}
};
