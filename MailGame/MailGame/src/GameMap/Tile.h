#pragma once
#include <memory>
#include <optional>
#include "System/IsoRotation/IsoRotation.h"

// Forward declaration
class Entity;

// The three types of tiles
enum class TileType {
	Land,
	Water,
	OffMap
};

// The railway on a tile
struct Railway {
	// The two directions the railway goes through
	IsoRotation from;
	IsoRotation to;
	Railway(IsoRotation from, IsoRotation to) : from(from), to(to) {}
};
// The road on a tile
struct Road {
	// Whether it has road going in any of the 4 directions
	bool hasNorth;
	bool hasEast;
	bool hasSouth;
	bool hasWest;
	Road() : hasNorth(false), hasEast(false), hasSouth(false), hasWest(false) {}
};

struct Tile {
	// The type of tile
	TileType type;
	// The building on the tile
	std::weak_ptr<Entity> building;
	// The postal code assigned to the tile
	long long postalCode;
	// The railway, if it has one
	std::optional<Railway> railway;
	// The road, if it has one
	std::optional<Road> road;

	Tile(TileType t = TileType::Land): postalCode(0) {
		type = t;
	}
};
