#include "ConstructionLayout.h"
#include "GameMap/GameMap.h"

ConstructionLayout::ConstructionLayout(std::vector<std::vector<TileType>> l) : layout(l) {}

bool ConstructionLayout::fits(GameMap* gMap, sf::Vector2i pos) {
	for (size_t x = 0; x < this->layout.size(); x++) {
		for (size_t y = 0; y < this->layout[x].size(); y++) {
			if (this->layout[x][y] == TileType::Water) {
				size_t pointsUnderwater = 0;
				for (size_t i = 0; i < 2; i++) {
					for (size_t j = 0; j < 2; j++) {
						if (gMap->getPointHeight(pos.x + x + i, pos.y + y + j) < GameMap::SEA_LEVEL) {
							pointsUnderwater++;
						}
					}
				}
				if (pointsUnderwater < 2) {
					return false;
				}
			}
			else {
				if (gMap->getTileAt(pos.x + x, pos.y + y).type != this->layout[x][y]) {
					return false;
				}
			}
		}
	}
	return true;
}
sf::Vector2i ConstructionLayout::getSize() {
	return sf::Vector2i(this->layout.size(), this->layout[0].size());
}
TileType ConstructionLayout::getTypeAt(size_t x, size_t y) {
	if (x >= 0 && x < this->layout.size()) {
		if (y >= 0 && y < this->layout[x].size()) {
			return this->layout[x][y];
		}
	}
	return TileType::OffMap;
}

ConstructionLayout ConstructionLayout::rotate(ConstructionLayout l, size_t n) {
	for (size_t i = 0; i < n; i++) {
		// Get size
		sf::Vector2i size = l.getSize();
		// Create and resize new layout
		std::vector<std::vector<TileType>> newLayout;
		newLayout.resize(size.y);
		for (auto it = newLayout.begin(); it != newLayout.end(); it++) {
			it->resize(size.x);
		}
		// Copy data
		for (size_t x = 0; x < size.x; x++) {
			for (size_t y = 0; y < size.y; y++) {
				// Fix rounding error
				size_t a = (int)size.y - 1 - y;
				newLayout[a][x] = l.getTypeAt(x, y);
			}
		}
		// Replace
		l = newLayout;
	}
	// Return new layout
	return ConstructionLayout(l);
}