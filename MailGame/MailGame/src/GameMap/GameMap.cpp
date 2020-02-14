#include "GameMap\GameMap.h"
#include "Game\Game.h"
#include "Entity/Entity.h"
#include "Component/Renderer/Renderer.h"
#include "ResourceLoader\ResourceLoader.h"
#include "System/Utils/Utils.h"
#include "Line/Line.h"
#include "Entity/EntityPresets/EntityPresets.h"
#include "PostalCodeDatabase/PostalCodeDatabase.h"
#include <SFML/Graphics.hpp>
#include <queue>
#include <stdlib.h>
#include <bitset>
#include <algorithm>

const size_t GameMap::MAP_HEIGHT = 50;
const size_t GameMap::MAP_WIDTH = 50;

// Load the sprites
const sf::Sprite GameMap::EMPTY_SPRITE = ResourceLoader::get()->getSprite("tiles/tiles", "empty");
std::vector<sf::Sprite> GameMap::ROAD_SPRITES;
std::vector<sf::Sprite> GameMap::RAIL_TRACK_SPRITES;

GameMap::GameMap(Game* g): game(g) {
	for (int i = 0; i < 16; i++) {
		GameMap::ROAD_SPRITES.push_back(ResourceLoader::get()->getSprite("tiles/tiles", "road-" + std::bitset<4>(i).to_string()));
		GameMap::RAIL_TRACK_SPRITES.push_back(ResourceLoader::get()->getSprite("tiles/tiles", "railway-" + std::bitset<4>(i).to_string()));
	}

	// Add the first postal code
	long long pCode = PostalCodeDatabase::get()->createPostalCode({ sf::Color(255, 0, 0, 100) });
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
	int rotation = this->game->getRotation().getRotation();
	switch(rotation) {
	case IsoRotation::NORTH:
		for (size_t x = 0; x < MAP_WIDTH; x++) {
			for (size_t y = 0; y < MAP_HEIGHT; y++) {
				renderTile(window, x, y);
			}
		}
		break;
	case IsoRotation::EAST:
		for (size_t x = 0; x < MAP_WIDTH; x++) {
			for (int y = MAP_HEIGHT - 1; y >= 0; y--) {
				renderTile(window, x, (size_t)y);
			}
		}
		break;
	case IsoRotation::SOUTH:
		for (int x = MAP_WIDTH - 1; x >= 0; x--) {
			for (int y = MAP_HEIGHT - 1; y >= 0; y--) {
				renderTile(window, (size_t)x, (size_t)y);
			}
		}
		break;
	case IsoRotation::WEST:
		for (int x = MAP_WIDTH - 1; x >= 0; x--) {
			for (size_t y = 0; y < MAP_HEIGHT; y++) {
				renderTile(window, (size_t)x, y);
			}
		}
	}
}
void GameMap::renderTile(sf::RenderWindow* window, size_t x, size_t y) {
	if (auto e = this->tiles[x][y].building.lock()) {
		// A bit messy, but hopefully this will be removed before the final product
		// Basically use a whitelist of entity tags which should be drawn instead of tiles
		// as opposed to over them
		const std::vector<EntityTag> v = Game::WHITELIST_ENTITY_TAG;
		if (std::find(v.begin(), v.end(), e->tag) != v.end()) {
			e->renderer->render(window);
			return;
		}
	}
	sf::Sprite s;
	switch (tiles[x][y].type) {
	case TileType::Empty:
		if (!tiles[x][y].hasRailTrack) {
			s = EMPTY_SPRITE;
		} else {
			int index = 0;
			if (getTileAt(x, y - 1).hasRailTrack) {
				index |= 0b1000;
			}
			if (getTileAt(x + 1, y).hasRailTrack) {
				index |= 0b0100;
			}
			if (getTileAt(x, y + 1).hasRailTrack) {
				index |= 0b0010;
			}
			if (getTileAt(x - 1, y).hasRailTrack) {
				index |= 0b0001;
			}
			unsigned int rotation = game->getRotation().getRotation();
			index = 0b1111 & ((index >> rotation) | (index << (4 - rotation)));
			// Get the sprite
			s = RAIL_TRACK_SPRITES[index];
		}
		break;
	case TileType::Road:
		int index = 0;
		if (getTileAt(x, y - 1).type == TileType::Road) {
			index |= 0b1000;
		}
		if (getTileAt(x + 1, y).type == TileType::Road) {
			index |= 0b0100;
		}
		if (getTileAt(x, y + 1).type == TileType::Road) {
			index |= 0b0010;
		}
		if (getTileAt(x - 1, y).type == TileType::Road) {
			index |= 0b0001;
		}
		unsigned int rotation = game->getRotation().getRotation();
		index = 0b1111 & ((index >> rotation) | (index << (4 - rotation)));
		s = ROAD_SPRITES[index];
		break;
	}
	sf::Vector2f pos((float)x + 0.5f, (float)y + 0.5f);
	s.setPosition(this->game->worldToScreenPos(pos));
	s = Utils::setupBuildingSprite(s, false);
	window->draw(s);
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
	// Have a 90% change that there is a building on each tile adjacent to a road
	for (size_t x = 0; x < MAP_WIDTH; x++) {
		for (size_t y = 0; y < MAP_HEIGHT; y++) {
			if (this->tiles[x][y].type == TileType::Road) {
				for (int xOff = -1; xOff < 2; xOff++) {
					for (int yOff = -1; yOff < 2; yOff++) {
						if (xOff == 0 || yOff == 0) {
							if (this->getTileAt(x + xOff, y + yOff).type == TileType::Empty) {
								if (rand() % 10 == 9)
									continue;
								IsoRotation rot;
								switch (xOff) {
								case -1:
									rot = IsoRotation::EAST;
									break;
								case 1:
									rot = IsoRotation::WEST;
									break;
								case 0:
									rot = yOff == 1 ? IsoRotation::NORTH: IsoRotation::SOUTH;
								}
								this->tiles[x + xOff][y + yOff].type = TileType::House;
								std::shared_ptr<Entity> e = EntityPresets::residence(this->game, sf::Vector2f((float)(x + xOff), (float)(y + yOff)), rot);
								this->game->addEntity(
									e
								);
								this->tiles[x + xOff][y + yOff].building = e;
							}
						}
					}
				}
			}
		}
	}
}

void GameMap::setBuildingForTile(size_t x, size_t y, std::weak_ptr<Entity> building) {
	if (this->getTileAt(x, y).type != TileType::OffMap) {
		this->tiles[x][y].building = building;
	}
}
void GameMap::setCodeForTile(size_t x, size_t y, long long id) {
	if (this->getTileAt(x, y).type != TileType::OffMap) {
		this->tiles[x][y].postalCode = id;
	}
}
void GameMap::addRailTrack(size_t x, size_t y) {
	if (this->getTileAt(x, y).type != TileType::OffMap) {
		this->tiles[x][y].hasRailTrack = true;
	}
}

Tile GameMap::getTileAt(size_t x, size_t y) {
	if (x <= tiles.size() && y <= tiles[0].size()) {
		return tiles[x][y];
	}
	return Tile(TileType::OffMap);
}
