#pragma once
#include <memory>

// Forward declaration
class Entity;

enum TileType {
	Empty,
	Road,
	House,
	OffMap
};

struct Tile {
	TileType type;
	std::weak_ptr<Entity> building;

	Tile(TileType t = TileType::Empty) {
		type = t;
	}
};
