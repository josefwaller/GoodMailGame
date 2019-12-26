#include "GameMap\GameMap.h"
#include "ResourceLoader\ResourceLoader.h"
#include "Line/Line.h"
#include <SFML/Graphics.hpp>
#include <queue>
#include <stdlib.h>

const size_t GameMap::MAP_HEIGHT = 50;
const size_t GameMap::MAP_WIDTH = 50;

// Load the sprites
sf::Sprite GameMap::EMPTY_SPRITE = ResourceLoader::get()->getSprite("tiles/tiles", "empty");
sf::Sprite GameMap::ROAD_SPRITE = ResourceLoader::get()->getSprite("tiles/tiles", "road");

GameMap::GameMap(Game* g) {
	// Initialize a 20x20 grid of nothing
	for (size_t i = 0; i < MAP_WIDTH; i++) {
		tiles.push_back(std::vector<Tile>());
		for (size_t j = 0; j < MAP_HEIGHT; j++) {
			tiles[i].push_back(Tile());
		}
	}
	generateCityAt({ MAP_WIDTH / 2, MAP_HEIGHT / 4 });
}

void GameMap::render(sf::RenderWindow* window) {
	for (int x = 0; x < MAP_WIDTH; x++) {
		for (int y = 0; y < MAP_HEIGHT; y++) {
			sf::Sprite s;
			switch (tiles[x][y].type) {
			case TileType::Empty:
				s = EMPTY_SPRITE;
				break;
			case TileType::Road:
				s = ROAD_SPRITE;
				break;
			}
			// How much to move the file over and down for each x, y coordinate increase
			// Basically the distance from the center of the tile to the side or bottom,
			// but not quite since the tile image has extra stuff at the bottom
			const float X_OFFSET = 64.0f;
			const float Y_OFFSET = 32.0f;
			s.setPosition(sf::Vector2f(X_OFFSET * (x - y), Y_OFFSET * (x + y)));
			window->draw(s);
		}
	}
}
/*
 * A very simple algorithm which just generates n roads criss-crossing
 * around a position given
 */
void GameMap::generateCityAt(sf::Vector2i pos) {
	const size_t MIN_ROAD_LEN = 4;
	const size_t MAX_ROAD_LEN = 12;
	const size_t NUM_ROADS = 20;
	// How many roads to propose branching off of a road
	const size_t NUM_ROADS_PROPOSED = 5;

	// TBA: Has this passed down from App so the player can set the seed
	srand(50);
	
	std::vector<Line> addedLines;
	// Add the initial road
	std::queue<Line> potentialLines;
	unsigned initLen = rand() % (MAX_ROAD_LEN - MIN_ROAD_LEN) + MIN_ROAD_LEN;
	potentialLines.push(Line(sf::Vector2i(pos.x - initLen / 2, pos.y), initLen, false));

	for (int i = 0; i < NUM_ROADS; i++) {
		BEGIN_LOOP:
		// Get the top line
		Line topLine = potentialLines.front();
		potentialLines.pop();

		// Check the line is valid
		for (auto it = addedLines.begin(); it != addedLines.end(); it++) {
			if (topLine.isNextTo(*it)) {
				// Try another line
				goto BEGIN_LOOP;
			}
		}

		// Add the line
		addedLines.push_back(topLine);
		// Add some potential lines
		for (int j = 0; j < NUM_ROADS_PROPOSED; j++) {
			unsigned newLineLen = rand() % (MAX_ROAD_LEN - MIN_ROAD_LEN) + MIN_ROAD_LEN;
			bool newLineVert = !topLine.getIsVertical();
			sf::Vector2i newLineStart;
			// Get some random point along the current line
			// rand() % topLine.getLength() to get some point along topLine, and -rand() % newLineLen to shift newLine across topLine to form an intersection
			if (topLine.getIsVertical()) {
				newLineStart = topLine.getStart() + sf::Vector2i(- (int)(rand() % newLineLen), rand() % topLine.getLength());
			}
			else {
				newLineStart = topLine.getStart() + sf::Vector2i(rand() % topLine.getLength(), -(int)(rand() % newLineLen));
			}
			potentialLines.push(Line(newLineStart, newLineLen, newLineVert));
		}
	}
	// Set the tiles along each line into roads
	for (auto it = addedLines.begin(); it != addedLines.end(); it++) {
		for (size_t i = 0; i < it->getLength(); i++) {
			if (it->getIsVertical()) {
				if (it->getStart().y + i < MAP_HEIGHT)
					this->tiles[it->getStart().x][it->getStart().y + i].type = TileType::Road;
			}
			else {
				if (it->getStart().x + i < MAP_WIDTH)
					this->tiles[it->getStart().x + i][it->getStart().y].type = TileType::Road;
			}
		}
	}
}

Tile GameMap::getTileAt(size_t x, size_t y) {
	if (x <= tiles.size() && y <= tiles[0].size()) {
		return tiles[x][y];
	}
	return Tile(TileType::Empty);
}