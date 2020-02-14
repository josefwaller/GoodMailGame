#pragma once
#include <memory>

// Forward declaration
class Entity;

enum class TileType {
	Empty,
	Road,
	House,
	OffMap
};

struct Tile {
	// The type of tile
	TileType type;
	// The building on the tile
	std::weak_ptr<Entity> building;
	// The postal code assigned to the tile
	long long postalCode;
	// Does the tile have a railway on it
	bool hasRailTrack;

	Tile(TileType t = TileType::Empty): postalCode(0), hasRailTrack(false) {
		type = t;
	}
};
