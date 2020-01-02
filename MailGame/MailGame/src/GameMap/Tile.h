#pragma once

enum TileType {
	Empty,
	Road,
	House,
	OffMap
};

struct Tile {
	TileType type;

	Tile(TileType t = TileType::Empty) {
		type = t;
	}
};
