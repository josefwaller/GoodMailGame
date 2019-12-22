#include "GameMap\GameMap.h"
#include "ResourceLoader\ResourceLoader.h"
#include <SFML/Graphics.hpp>

GameMap::GameMap(Game* g) {
	// Initialize a 20x20 grid of nothing
	for (size_t i = 0; i < MAP_WIDTH; i++) {
		tiles.push_back(std::vector<Tile>());
		for (size_t j = 0; j < MAP_HEIGHT; j++) {
			tiles[i].push_back(Tile());
		}
	}
}

void GameMap::render(sf::RenderWindow* window) {
	sf::Sprite s = ResourceLoader::get()->getSprite("tiles/tiles", "empty");
	window->draw(s);
}

Tile GameMap::getTileAt(size_t x, size_t y) {
	if (x <= tiles.size() && y <= tiles[0].size()) {
		return tiles[x][y];
	}
	return Tile(TileType::Empty);
}