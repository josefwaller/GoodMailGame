#pragma once
#include "GameMap/Tile/Tile.h"
#include <SFML/System/Vector2.hpp>
#include <vector>

class GameMap;

/*
 * A basic wrapper around a 2D vector of TileTypes
 * Used for construction, to determine if a position is a valid place for a building
 */
class ConstructionLayout {
public:
	// Create with given layout
	ConstructionLayout(std::vector<std::vector<TileType>> layout);
	// Check if the position fits the layout
	bool fits(GameMap* gMap, sf::Vector2i pos);
	// Get the size of the layout
	sf::Vector2i getSize();
	// Gets the type of the tile at the position, or TileType::OffMap if the position is invalid
	TileType getTypeAt(size_t x, size_t y);
	// Get a new layout which is rotated 90 degrees n times
	static ConstructionLayout rotate(ConstructionLayout lay, size_t n);
private:
	std::vector<std::vector<TileType>> layout;
};
