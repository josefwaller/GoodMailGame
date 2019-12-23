#include "GameMap\GameMap.h"
#include "ResourceLoader\ResourceLoader.h"
#include <SFML/Graphics.hpp>

const size_t GameMap::MAP_HEIGHT = 50;
const size_t GameMap::MAP_WIDTH = 50;

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
	for (int x = 0; x < MAP_WIDTH; x++) {
		for (int y = 0; y < MAP_HEIGHT; y++) {
			sf::Sprite s;
			switch (tiles[x][y].type) {
			case TileType::Empty:
				s = ResourceLoader::get()->getSprite("tiles/tiles", "empty");
				break;
			case TileType::Road:
				s = ResourceLoader::get()->getSprite("tiles/tiles", "road");
				break;
			}
			// Scale of image
			float scale = 0.3f;
			// How much to move the file over and down for each x, y coordinate increase
			// Basically the distance from the center of the tile to the side or bottom,
			// but not quite since the tile image has extra stuff at the bottom
			const float X_OFFSET = 64.0f;
			const float Y_OFFSET = 32.0f;
			s.setScale(scale, scale);
			s.setPosition(sf::Vector2f(scale * X_OFFSET * (x - y), scale * Y_OFFSET * (x + y)));
			window->draw(s);
		}
	}
}

Tile GameMap::getTileAt(size_t x, size_t y) {
	if (x <= tiles.size() && y <= tiles[0].size()) {
		return tiles[x][y];
	}
	return Tile(TileType::Empty);
}