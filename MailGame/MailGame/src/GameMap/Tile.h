#pragma once
#include <memory>
#include <optional>
#include "System/IsoRotation/IsoRotation.h"

// Forward declaration
class Entity;

enum class TileType {
	Empty,
	Road,
	House,
	OffMap
};

// The railway on a tile
struct Railway {
	// The two directions the railway goes through
	IsoRotation from;
	IsoRotation to;
	Railway(IsoRotation from, IsoRotation to) : from(from), to(to) {}
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

	Tile(TileType t = TileType::Empty): postalCode(0) {
		type = t;
	}
};
