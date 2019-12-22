#pragma once

enum TileType {
	Empty,
	Road,
	House,
};

struct Tile {
	TileType type;

	Tile(TileType t = TileType::Empty) {
		type = t;
	}
};
