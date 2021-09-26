#include "GameMap\GameMap.h"
#include "Game\Game.h"
#include "Entity/Entity.h"
#include "Component/Renderer/Renderer.h"
#include "ResourceLoader\ResourceLoader.h"
#include "System/Utils/Utils.h"
#include "Line/Line.h"
#include "Entity/EntityPresets/BuildingPresets/BuildingPresets.h"
#include "PostalCodeDatabase/PostalCodeDatabase.h"
#include <FastNoiseLite.h>
#include <SFML/Graphics.hpp>
#include "System/SaveData/SaveData.h"
#include <queue>
#include <stdlib.h>
#include <bitset>
#include <algorithm>

const size_t GameMap::MAP_HEIGHT = 100;
const size_t GameMap::MAP_WIDTH = 100;

const unsigned int GameMap::MIN_HEIGHT = 0;
const unsigned int GameMap::MAX_HEIGHT = 10;

const unsigned int GameMap::SEA_LEVEL = 4;

// Load the sprites
const sf::Sprite GameMap::EMPTY_SPRITE = ResourceLoader::get()->getSprite("tiles/tiles", "empty");
std::vector<sf::Sprite> GameMap::RAIL_TRACK_SPRITES;

GameMap::GameMap(Game* g) : game(g) {
	// Load the tile sprites
	for (int i = 0; i < 16; i++) {
		GameMap::RAIL_TRACK_SPRITES.push_back(ResourceLoader::get()->getSprite("tiles/tiles", "railway-" + std::bitset<4>(i).to_string()));
	}
	// Add the first postal code
	long long pCode = PostalCodeDatabase::get()->createPostalCode("Postal Code 0", sf::Color(255, 0, 0, 100));
	// Initialize a grid of nothing
	for (size_t i = 0; i < MAP_WIDTH; i++) {
		tiles.push_back(std::vector<Tile>());
		for (size_t j = 0; j < MAP_HEIGHT; j++) {
			tiles[i].push_back(Tile());
		}
	}
	for (size_t i = 0; i < MAP_WIDTH + 1; i++) {
		this->pointHeights.push_back({});
		for (size_t j = 0; j < MAP_HEIGHT + 1; j++) {
			this->pointHeights[i].push_back(0);
		}
	}
}
void GameMap::generateNew() {
	FastNoiseLite noise(18);
	noise.SetNoiseType(FastNoiseLite::NoiseType::NoiseType_Perlin);
	noise.SetFrequency(0.07f);
	noise.SetFractalType(FastNoiseLite::FractalType::FractalType_FBm);
	noise.SetFractalOctaves(1);
	noise.SetFractalLacunarity(0.1f);
	noise.SetFractalGain(0.5f);

	// Set up point heights
	this->pointHeights = std::vector<std::vector<unsigned int>>(MAP_WIDTH + 1, std::vector<unsigned int>(MAP_HEIGHT + 1, 0.0f));
	this->pointHeightLocked = std::vector<std::vector<bool>>(MAP_WIDTH + 1, std::vector<bool>(MAP_HEIGHT + 1, false));
	for (size_t i = 0; i < MAP_WIDTH + 1; i++) {
		for (size_t j = 0; j < MAP_HEIGHT + 1; j++) {
			float n = noise.GetNoise((float)i, (float)j);
			unsigned int val = (unsigned int)abs(round((float)(MAX_HEIGHT - MIN_HEIGHT) * (n + 1.0f) / 2) + MIN_HEIGHT);
			pointHeights.at(i).at(j) = val;
		}
	}
	// Generate city
	generateCityAt({ MAP_WIDTH / 4, MAP_HEIGHT / 4 }, 1);
	generateCityAt({ MAP_WIDTH * 3 / 4, MAP_HEIGHT * 3 / 4 }, 2);
	generateCityAt({ MAP_WIDTH * 3 / 4, MAP_HEIGHT / 4 }, 3);
}
void GameMap::render(sf::RenderWindow* window) {
	// Render all the tiles on screen
	int rotation = this->game->getRotation().getRotation();
	switch (rotation) {
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
	// Render tunnels
	for (auto it = this->tunnels.begin(); it != this->tunnels.end(); it++) {
		it->render(window);
	}
}

sf::Sprite GameMap::getRoadSprite(Road road, IsoRotation gameRotation, std::vector<std::vector<unsigned int>> heights) {
	std::string spriteString = "road_";
	sf::Vector2f origin;
	if (heights[0][0] != heights[0][1] || heights[0][0] != heights[1][0]) {
		// Must be a ramp
		spriteString += "slantFlat_";
		origin = sf::Vector2f(255, 304);
		if (heights[0][0] > heights[0][1]) {
			spriteString += "SE";
		}
		else if (heights[0][0] < heights[0][1]) {
			spriteString += "NW";
		}
		else if (heights[0][0] > heights[1][0]) {
			spriteString += "NE";
		}
		else if (heights[0][0] < heights[1][0]) {
			spriteString += "SW";
		}
		else {
			throw std::runtime_error("Tiles not set up properly for roads");
		}
	}
	else {
		origin = sf::Vector2f(256, 314);
		// Get number of outgoing road connections
		size_t c = (road.hasNorth ? 1 : 0) + (road.hasEast ? 1 : 0) + (road.hasSouth ? 1 : 0) + (road.hasWest ? 1 : 0);
		// Check if 4 way intersection
		if (c == 4) {
			spriteString += "crossroad_NE";
		}
		// Check if the road is a 3 way intersection
		if (c == 3) {
			spriteString += "intersection_";
			// Set the sprite name
			if (!road.hasNorth) {
				spriteString += "SW";
			}
			else if (!road.hasEast) {
				spriteString += "NW";
			}
			else if (!road.hasSouth) {
				spriteString += "NE";
			}
			else {
				spriteString += "SE";
			}
		}
		else if (c == 2) {
			// If the road is straight
			if (road.hasNorth && road.hasSouth) {
				spriteString += "straight_SE";
			}
			else if (road.hasEast && road.hasWest) {
				spriteString += "straight_SW";
			}
			else if (road.hasNorth && road.hasEast) {
				spriteString += "bend_NE";
			}
			else if (road.hasEast && road.hasSouth) {
				spriteString += "bend_SE";
			}
			else if (road.hasSouth && road.hasWest) {
				spriteString += "bend_SW";
			}
			else {
				spriteString += "bend_NW";
			}
		}
		else if (c == 1) {
			spriteString += "end_";
			if (road.hasNorth) {
				spriteString += "SE";
			}
			else if (road.hasEast) {
				spriteString += "SW";
			}
			else if (road.hasSouth) {
				spriteString += "NW";
			}
			else if (road.hasWest) {
				spriteString += "NE";
			}
		}
	}
	sf::Sprite spr = ResourceLoader::get()->getIndividualSprite("roadTiles/" + spriteString + ".png");
	spr.setOrigin(origin);
	return spr;
}
void GameMap::renderTile(sf::RenderWindow* window, size_t x, size_t y) {
	sf::Vector2f p = this->game->worldToScreenPos(sf::Vector3f(x, y, 0));
	sf::FloatRect r = this->game->getViewport();
	if (!r.contains(p)) {
		return;
	}
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
	Tile tile = tiles[x][y];
	bool drawSprite = true;
	switch (tile.type) {
	case TileType::Land:
		sf::Color tileColor = (x + y) % 2 == 0 ? sf::Color(0, 150, 0) : sf::Color(0, 175, 0);
		if (tile.hasRailwaySignal()) {
			tileColor = sf::Color(212, 0, 0);
		}
		sf::VertexArray arr(sf::PrimitiveType::Quads, 4);
		arr[0] = sf::Vertex(this->game->worldToScreenPos(sf::Vector3f(x, y, this->pointHeights.at(x).at(y))), tileColor);
		arr[1] = sf::Vertex(this->game->worldToScreenPos(sf::Vector3f(x + 1, y, this->pointHeights.at(x + 1).at(y))), tileColor);
		arr[2] = sf::Vertex(this->game->worldToScreenPos(sf::Vector3f(x + 1, y + 1, this->pointHeights.at(x + 1).at(y + 1))), tileColor);
		arr[3] = sf::Vertex(this->game->worldToScreenPos(sf::Vector3f(x, y + 1, this->pointHeights.at(x).at(y + 1))), tileColor);
		window->draw(arr);
		if (tile.road.has_value()) {
			std::vector<std::vector<unsigned int>> heights(2, std::vector<unsigned int>(2));
			for (size_t i = 0; i < 2; i++) {
				for (size_t j = 0; j < 2; j++) {
					heights[i][j] = this->pointHeights.at(x + i).at(y + j);
				}
			}
			s = getRoadSprite(tile.road.value(), this->game->getRotation(), heights);
		}
		if (tile.airplaneRoad.has_value()) {
			s = getRoadSprite(tile.airplaneRoad.value(), this->game->getRotation());
			s.setColor(tile.airplaneRoad.value().isRunway ? sf::Color::Blue : sf::Color::Red);
		}
	}
	if (drawSprite) {
		sf::Vector3f pos((float)x + 0.5f, (float)y + 0.5f, this->getHeightAt((float)x + 0.5f, (float)y + 0.5f));
		s.setPosition(this->game->worldToScreenPos(pos));
		if (!tile.canGetLock()) {
			s.setColor(sf::Color(0, 255, 51));
		}
		window->draw(s);
	}
	// Draw the railway if it has one
	if (!tile.getRailways().empty()) {
		for (Railway r : tile.getRailways()) {
			// Point is center of tile + the direction
			sf::Vector2f center = sf::Vector2f(x, y) + sf::Vector2f(0.5, 0.5);
			sf::Vector2f fromPoint = center + 0.5f * r.getFrom().getUnitVector();
			sf::Vector2f toPoint = center + 0.5f * r.getTo().getUnitVector();
			// Render line
			sf::VertexArray arr(sf::PrimitiveType::Lines, 2);
			arr[0] = sf::Vertex(this->game->worldToScreenPos(sf::Vector3f(fromPoint.x, fromPoint.y, this->getHeightAt(fromPoint))), r.getIsLocked() ? sf::Color::White : sf::Color::Black);
			arr[1] = sf::Vertex(this->game->worldToScreenPos(sf::Vector3f(toPoint.x, toPoint.y, this->getHeightAt(toPoint))), r.isStation ? sf::Color::Blue : sf::Color::Red);
			window->draw(arr);
		}
	}
	// Draw water on top of tile if it is underwater
	std::vector<sf::Vector3f> points;
	for (size_t i = 0; i < 2; i++) {
		for (size_t j = 0; j < 2; j++) {
			sf::Vector2i p(x + i, y + j);
			if (this->getPointHeight(p) <= SEA_LEVEL) {
				points.push_back(sf::Vector3f(p.x, p.y, SEA_LEVEL));
			}
		}
	}
	if (points.size() > 2) {
		sf::VertexArray arr(points.size() == 3 ? sf::PrimitiveType::Triangles : sf::PrimitiveType::Quads, points.size());
		// Have to fix the order if the entire tile is underwater
		if (points.size() > 3) {
			sf::Vector3f temp = points.at(3);
			points.at(3) = points.at(2);
			points.at(2) = temp;
		}
		for (size_t i = 0; i < points.size(); i++) {
			arr[i] = sf::Vertex(this->game->worldToScreenPos(points[i]), sf::Color(0, 0, 255, 150));
		}
		window->draw(arr);
	}
}
void GameMap::prepareTileForRoad(size_t x, size_t y, Road r) {
	if (r.hasNorth || r.hasSouth) {
		for (size_t yOff = 0; yOff < 2; yOff++) {
			unsigned int height = std::min(this->pointHeights.at(x).at(y + yOff), this->pointHeights.at(x + 1).at(y + yOff));
			this->pointHeights.at(x).at(y + yOff) = height;
			this->pointHeights.at(x + 1).at(y + yOff) = height;
		}
	}
	if (r.hasEast || r.hasWest) {
		for (size_t xOff = 0; xOff < 2; xOff++) {
			unsigned int height = std::min(this->pointHeights.at(x + xOff).at(y), this->pointHeights.at(x + xOff).at(y + 1));
			this->pointHeights.at(x + xOff).at(y) = height;
			this->pointHeights.at(x + xOff).at(y + 1) = height;
		}
	}
}
/*
 * A very simple algorithm which just generates n roads criss-crossing
 * around a position given
 */
void GameMap::generateCityAt(sf::Vector2i pos, id_t cityId) {
	const size_t MIN_ROAD_LEN = 4;
	const size_t MAX_ROAD_LEN = 12;
	const size_t NUM_ROADS = 20;
	// How many roads to propose branching off of a road
	const size_t NUM_ROADS_PROPOSED = 5;

	// TBA: Has this passed down from App so the player can set the seed
	srand(18);

	std::vector<Line> addedLines;
	// Add the initial road
	std::queue<Line> potentialLines;
	unsigned initLen = rand() % (MAX_ROAD_LEN - MIN_ROAD_LEN) + MIN_ROAD_LEN;
	potentialLines.push(Line(sf::Vector2i(pos.x - initLen / 2, pos.y), initLen, false));

	// List of all the tiles a road has been added to
	std::vector<sf::Vector2i> addedRoads;

	for (int i = 0; i < NUM_ROADS; i++) {
	BEGIN_LOOP:
		// For right now, if we don't have any lines, just finish generating the city
		if (potentialLines.empty()) {
			break;
		}
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
		// Make the line valid for a road
		for (size_t i = 0; i < topLine.getLength(); i++) {
			if (i == 0 || i == topLine.getLength() - 1) {
				// Tile must be completely flat
				sf::Vector2i unit = topLine.getIsVertical() ? sf::Vector2i(0, 1) : sf::Vector2i(1, 0);
				sf::Vector2i tile = topLine.getStart() + unit * (int)i;
				unsigned int height = this->getPointHeight(tile);
				for (size_t i = 0; i < 2; i++) {
					for (size_t j = 0; j < 2; j++) {
						this->setPointHeight(tile.x + i, tile.y + j, height);
					}
				}
			}
			else {
				std::pair<sf::Vector2i, sf::Vector2i> pairOne;
				std::pair<sf::Vector2i, sf::Vector2i> pairTwo;
				if (topLine.getIsVertical()) {
					sf::Vector2i point = topLine.getStart() + (int)i * sf::Vector2i(0, 1);
					pairOne = { point, point + sf::Vector2i(1, 0) };
					pairTwo = { point + sf::Vector2i(0, 1), point + sf::Vector2i(1, 1) };
				}
				else {
					sf::Vector2i point = topLine.getStart() + (int)i * sf::Vector2i(1, 0);
					pairOne = { point, point + sf::Vector2i(0, 1) };
					pairTwo = { point + sf::Vector2i(1, 0), point + sf::Vector2i(1, 1) };
				}
				// Make sure pair one and pair two are the same heights
				for (auto pair : { pairOne, pairTwo }) {
					if (pair.first == sf::Vector2i(31, 27) || pair.second == sf::Vector2i(31, 27)) {
						auto x = 0;
					}
					if (this->getPointHeight(pair.first) != this->getPointHeight(pair.second)) {
						// Set them to the same height
						this->setPointHeight(pair.second.x, pair.second.y, this->getPointHeight(pair.first));
					}
					this->pointHeightLocked.at(pair.first.x).at(pair.first.y) = true;
					this->pointHeightLocked.at(pair.second.x).at(pair.second.y) = true;
				}
			}
		}
		// Add some potential lines
		for (int j = 0; j < NUM_ROADS_PROPOSED; j++) {
			unsigned newLineLen = rand() % (MAX_ROAD_LEN - MIN_ROAD_LEN) + MIN_ROAD_LEN;
			bool newLineVert = !topLine.getIsVertical();
			sf::Vector2i newLineStart;
			// Get some random point along the current line
			// rand() % topLine.getLength() to get some point along topLine, and -rand() % newLineLen to shift newLine across topLine to form an intersection
			if (topLine.getIsVertical()) {
				newLineStart = topLine.getStart() + sf::Vector2i(-(int)(rand() % newLineLen), rand() % topLine.getLength());
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
				if (it->getStart().y + i < MAP_HEIGHT) {
					sf::Vector2i pos(it->getStart().x, it->getStart().y + i);
					Tile* t = &this->tiles[it->getStart().x][it->getStart().y + i];
					t->cityId = cityId;
					Road r;
					if (t->road.has_value()) {
						r = t->road.value();
					}
					if (i != it->getLength() - 1)
						r.hasSouth = true;
					if (i != 0)
						r.hasNorth = true;
					t->road = r;
					addedRoads.push_back({ (int)it->getStart().x, (int)(it->getStart().y + i) });
				}
			}
			else {
				if (it->getStart().x + i < MAP_WIDTH) {
					sf::Vector2i pos(it->getStart().x + 1, it->getStart().y);
					Tile* t = &this->tiles[it->getStart().x + i][it->getStart().y];
					t->cityId = cityId;
					Road r;
					if (t->road.has_value()) {
						r = t->road.value();
					}
					if (i != it->getLength() - 1)
						r.hasEast = true;
					if (i != 0)
						r.hasWest = true;
					t->road = r;
					addedRoads.push_back({ (int)(it->getStart().x + i), (int)(it->getStart().y) });
				}
			}
		}
	}
	// Have a 90% change that there is a building on each tile adjacent to a road
	for (size_t i = 0; i < addedRoads.size(); i++) {
		size_t x = addedRoads.at(i).x;
		size_t y = addedRoads.at(i).y;
		if (this->hasRoadAt(x, y)) {
			for (int xOff = -1; xOff < 2; xOff++) {
				for (int yOff = -1; yOff < 2; yOff++) {
					if (xOff == 0 || yOff == 0) {
						if (!this->hasRoadAt(x + xOff, y + yOff)) {
							if (this->getTileAt(x + xOff, y + yOff).type == TileType::OffMap) {
								continue;
							}
							if (rand() % 10 == 9)
								continue;
							IsoRotation rot;
							unsigned int height;
							switch (xOff) {
							case -1:
								rot = IsoRotation::EAST;
								height = this->pointHeights.at(x + 1).at(y);
								break;
							case 1:
								rot = IsoRotation::WEST;
								height = this->pointHeights.at(x).at(y);
								break;
							case 0: {
								if (yOff == 1) {
									rot = IsoRotation::NORTH;
									height = this->pointHeights.at(x).at(y);
								}
								else {
									rot = IsoRotation::SOUTH;
									height = this->pointHeights.at(x).at(y + 1);
								}
							}
							}
							// check that we aren't messing up hte heights
							bool shouldSkip = false;
							for (size_t i = 0; i < 2; i++) {
								for (size_t j = 0; j < 2; j++) {
									if (this->pointHeights.at(x + xOff + i).at(y + yOff + j) != height && this->pointHeightLocked.at(x + xOff + i).at(y + yOff + j)) {
										shouldSkip = true;
									}
								}
							}
							if (shouldSkip) {
								continue;
							}
							// Set the height of the tile
							for (size_t i = 0; i < 2; i++) {
								for (size_t j = 0; j < 2; j++) {
									this->pointHeights.at(x + xOff + i).at(y + yOff + j) = height;
								}
							}
							std::shared_ptr<Entity> e = BuildingPresets::residence(this->game, sf::Vector3f((float)(x + xOff), (float)(y + yOff), (float)height), rot);
							this->game->addEntity(
								e
							);
							this->tiles[x + xOff][y + yOff].building = e;
							this->tiles[x + xOff][y + yOff].cityId = cityId;
							this->residenceLocations.push_back(sf::Vector2i(x + xOff, y + yOff));
						}
					}
				}
			}
		}
	}
}

unsigned int GameMap::getPointHeight(size_t x, size_t y) {
	if (x < this->pointHeights.size() && y < this->pointHeights.at(x).size()) {
		return this->pointHeights.at(x).at(y);
	}
	return 0;
}
unsigned int GameMap::getPointHeight(sf::Vector2i point) {
	return getPointHeight(point.x, point.y);
}

void GameMap::setPointHeight(size_t x, size_t y, unsigned int height) {
	if (x < this->pointHeights.size() && y < this->pointHeights.at(x).size()) {
		this->pointHeights.at(x).at(y) = height;
	}
}

float GameMap::getTileHeight(size_t x, size_t y) {
	unsigned int total = 0;
	for (size_t i = 0; i < 2; i++) {
		for (size_t j = 0; j < 2; j++) {
			total += this->getPointHeight(x + i, y + j);
		}
	}
	return (float)total / 4.0f;
}

float GameMap::getLowestTileHeight(size_t x, size_t y) {
	float min = 100.0f;
	for (size_t i = 0; i < 2; i++) {
		for (size_t j = 0; j < 2; j++) {
			if (min > this->getPointHeight(x + i, y + j)) {
				min = this->getPointHeight(x + i, y + j);
			}
		}
	}
	return min;
}

float GameMap::getHeightAt(sf::Vector2f p) {
	return getHeightAt(p.x, p.y);
}

float GameMap::getHeightAt(float x, float y) {
	// Get the points around the position to get the height
	size_t minX = floor(x);
	size_t maxX = minX + 1;
	size_t minY = floor(y);
	size_t maxY = minY + 1;

	unsigned int topLeft = this->getPointHeight(minX, minY);
	unsigned int botLeft = this->getPointHeight(minX, maxY);
	unsigned int topRight = this->getPointHeight(maxX, minY);
	unsigned int botRight = this->getPointHeight(maxX, maxY);

	// All points are equal, so just return height
	if (topLeft == botLeft && botLeft == topRight && topRight == botRight) {
		return (float)topLeft;
	}
	// Check for ramp
	// Top to bottom
	if (topLeft == botLeft && topRight == botRight) {
		return (float)(botLeft)+(x - (float)minX) * ((float)botRight - (float)botLeft);
	}
	// Left to right
	else if (topLeft == topRight && botLeft == botRight) {
		return (float)(topLeft)+(y - (float)minY) * ((float)botLeft - (float)topLeft);
	}
	// TBA
	return 0.0f;
}

bool GameMap::canGetTileLock(size_t x, size_t y, TransitType type) {
	return !this->getTileAt(x, y).isLocked;
}

void GameMap::getTileLock(size_t x, size_t y, TransitType type) {
	if (this->getTileAt(x, y).isLocked) {
		throw std::runtime_error("Tile is already locked");
	}
	this->tiles[x][y].isLocked = true;
}
void GameMap::releaseTileLock(size_t x, size_t y, TransitType type) {
	if (this->getTileAt(x, y).isLocked) {
		this->tiles[x][y].isLocked = false;
	}
}
bool GameMap::canGetRailwayLock(sf::Vector2i tile, Railway r) {
	if (this->getTileAt(tile).type == TileType::OffMap) {
		return false;
	}
	return this->tiles[tile.x][tile.y].canGetRailwayLock(r);
}
void GameMap::getRailwayLock(sf::Vector2i tile, Railway r) {
	this->tiles[tile.x][tile.y].getRailwayLock(r);
}
void GameMap::releaseRailwayLock(sf::Vector2i tile, Railway r) {
	this->tiles[tile.x][tile.y].releaseRailwayLock(r);
}
bool GameMap::canGetTunnelLock(Tunnel t) {
	for (auto it = this->tunnels.begin(); it != this->tunnels.end(); it++) {
		if (*it == t) {
			return it->canGetLock();
		}
	}
}
void GameMap::getTunnelLock(Tunnel t) {
	for (auto it = this->tunnels.begin(); it != this->tunnels.end(); it++) {
		if (*it == t) {
			return it->getLock();
		}
	}
}
void GameMap::releaseTunnelLock(Tunnel t) {
	for (auto it = this->tunnels.begin(); it != this->tunnels.end(); it++) {
		if (*it == t) {
			return it->releaseLock();
		}
	}
}
void GameMap::setBuildingForTile(size_t x, size_t y, std::weak_ptr<Entity> building) {
	if (this->getTileAt(x, y).type != TileType::OffMap) {
		this->tiles[x][y].building = building;
	}
}
void GameMap::setCodeForTile(size_t x, size_t y, id_t id) {
	if (this->getTileAt(x, y).type != TileType::OffMap) {
		this->tiles[x][y].postalCode = id;
	}
}
void GameMap::addRoad(size_t x, size_t y, Road r) {
	if (this->getTileAt(x, y).type != TileType::OffMap) {
		this->tiles.at(x).at(y).road = r;
	}
}
void GameMap::removeRoad(size_t x, size_t y) {
	if (this->getTileAt(x, y).type != TileType::OffMap) {
		this->tiles.at(x).at(y).road.reset();
	}
}
void GameMap::addRailTrack(size_t x, size_t y, IsoRotation from, IsoRotation to) {
	if (this->getTileAt(x, y).type != TileType::OffMap) {
		this->tiles[x][y].addRailway(Railway(from, to));
	}
}
void GameMap::removeRailTrack(size_t x, size_t y) {
	this->tiles[x][y].deleteRailways();
}

void GameMap::addRailwayStation(size_t x, size_t y, IsoRotation direction) {
	if (this->getTileAt(x, y).type != TileType::OffMap) {
		if (this->tiles[x][y].getRailways().empty()) {
			this->tiles[x][y].addRailway(Railway(direction.getReverse(), direction, true));
		}
	}
}
void GameMap::removeTunnel(size_t x, size_t y) {
	if (this->getTileAt(x, y).type != TileType::OffMap) {
		auto tuns = this->tunnels;
		sf::Vector2i v(x, y);
		for (auto it = tuns.begin(); it != tuns.end(); it++) {
			if (Utils::toVector2i(it->getEntrances().first.getPosition()) == v || Utils::toVector2i(it->getEntrances().second.getPosition()) == v) {
				this->tunnels.erase(std::find(this->tunnels.begin(), this->tunnels.end(), *it));
			}
		}
	}
}

void GameMap::addRailwaySignal(sf::Vector2i tile) {
	if (this->getTileAt(tile).type != TileType::OffMap) {
		this->tiles[tile.x][tile.y].isRailwaySignal = true;
	}
}

void GameMap::addAirplaneRoad(size_t x, size_t y, AirplaneRoad road) {
	if (this->getTileAt(x, y).type != TileType::OffMap) {
		this->tiles[x][y].airplaneRoad = road;
	}
}
void GameMap::removeAirplaneRoad(size_t x, size_t y) {
	if (this->getTileAt(x, y).type != TileType::OffMap) {
		this->tiles[x][y].airplaneRoad.reset();
	}
}

Tile GameMap::getTileAt(size_t x, size_t y) {
	if (x < tiles.size() && y < tiles[0].size()) {
		return tiles[x][y];
	}
	return Tile(TileType::OffMap);
}
Tile GameMap::getTileAt(sf::Vector2i p) {
	return getTileAt((size_t)p.x, (size_t)p.y);
}
bool GameMap::hasRoadAt(size_t x, size_t y) {
	return this->getTileAt(x, y).road.has_value();
}
bool GameMap::hasRoadInDirection(size_t x, size_t y, IsoRotation rot) {
	Tile t = this->getTileAt(x, y);
	if (t.road.has_value()) {
		switch (rot.getRotation()) {
		case IsoRotation::NORTH:
			return t.road.value().hasNorth;
		case IsoRotation::SOUTH:
			return t.road.value().hasSouth;
		case IsoRotation::EAST:
			return t.road.value().hasEast;
		case IsoRotation::WEST:
			return t.road.value().hasWest;
		}
	}
	return false;
}
void GameMap::addRoadInDirection(size_t x, size_t y, IsoRotation rot) {
	Tile t = this->getTileAt(x, y);
	if (t.type != TileType::OffMap) {
		// Add a road if it doesn't exist
		Road r;
		if (t.road.has_value()) {
			r = t.road.value();
		}
		switch (rot.getRotation()) {
		case IsoRotation::NORTH:
			r.hasNorth = true;
			break;
		case IsoRotation::SOUTH:
			r.hasSouth = true;
			break;
		case IsoRotation::EAST:
			r.hasEast = true;
			break;
		case IsoRotation::WEST:
			r.hasWest = true;
			break;
		}
		this->tiles[x][y].road = r;
	}
}
std::vector<sf::Vector2i> GameMap::getResidences() {
	return this->residenceLocations;
}
void GameMap::addTunnel(sf::Vector2i tileOne, sf::Vector2i tileTwo, TransitType type) {
	// TODO: add a bunch of checking here
	this->tunnels.push_back(Tunnel(
		sf::Vector3i(tileOne.x, tileOne.y, this->getLowestTileHeight(tileOne.x, tileOne.y)),
		sf::Vector3i(tileTwo.x, tileTwo.y, this->getLowestTileHeight(tileTwo.x, tileTwo.y)),
		type,
		this->game
	));
	// Add roads
	this->tiles[tileOne.x][tileOne.y].road = Road(IsoRotation::fromUnitVector(sf::Vector2f(Utils::getUnitVector(tileOne - tileTwo))));
	this->tiles[tileTwo.x][tileTwo.y].road = Road(IsoRotation::fromUnitVector(sf::Vector2f(Utils::getUnitVector(tileTwo - tileOne))));
}
std::vector<Tunnel> GameMap::getTunnelsForTile(sf::Vector2i tile) {
	std::vector<Tunnel> toReturn;
	for (Tunnel t : this->tunnels) {
		for (TunnelEntrance e : { t.getEntrances().first, t.getEntrances().second }) {
			if (Utils::toVector2i(sf::Vector3f(e.getPosition()) + e.getDirection().getReverse().getUnitVector3D()) == tile) {
				toReturn.push_back(t);
				break;
			}
		}
	}
	return toReturn;
}
std::vector<Tunnel> GameMap::getTunnels(sf::Vector2i tile) {
	std::vector<Tunnel> toReturn;
	for (Tunnel t : this->tunnels) {
		if (Utils::toVector2i(t.getEntrances().first.getPosition()) == tile || Utils::toVector2i(t.getEntrances().second.getPosition()) == tile) {
			toReturn.push_back(t);
		}
	}
	return toReturn;
}
SaveData GameMap::getSaveData() {
	using namespace SaveKeys;
	SaveData sd(GAMEMAP);
	for (size_t x = 0; x < this->tiles.size(); x++) {
		for (size_t y = 0; y < this->tiles[x].size(); y++) {
			Tile t = this->tiles[x][y];
			SaveData td(TILE);
			// Add position
			td.addSizeT(X, x);
			td.addSizeT(Y, y);
			// Add postal code
			td.addSizeT(POSTAL_CODE, t.postalCode);
			// Add type
			td.addSizeT(TYPE, (size_t)t.type);
			// Add city
			td.addSizeT(CITY, t.cityId);
			if (t.building.lock()) {
				td.addSizeT(BUILDING, t.building.lock()->getId());
			}
			// Add data for road
			if (t.road.has_value()) {
				Road r = t.road.value();
				td.addData(r.getSaveData());
			}
			// Add data for railway
			if (!t.getRailways().empty()) {
				for (Railway r : t.getRailways()) {
					td.addData(r.getSaveData());
				}
			}
			// Add data for airplane road
			if (t.airplaneRoad.has_value()) {
				auto r = t.airplaneRoad.value();
				td.addData(r.getSaveData());
			}
			sd.addData(td);
		}
	}
	// Save heights
	for (size_t x = 0; x < this->pointHeights.size(); x++) {
		for (size_t y = 0; y < this->pointHeights.at(x).size(); y++) {
			SaveData s(POINT);
			s.addSizeT(X, x);
			s.addSizeT(Y, y);
			s.addSizeT(HEIGHT, this->pointHeights.at(x).at(y));
			sd.addData(s);
		}
	}
	// Save tunnels
	for (Tunnel t : this->tunnels) {
		sd.addData(t.getSaveData());
	}
	return sd;
}
void GameMap::loadFromSaveData(SaveData data) {
	using namespace SaveKeys;
	// Go through every tile data
	for (SaveData d : data.getDatas()) {
		if (d.getName() == TILE) {
			size_t x = d.getSizeT(X);
			size_t y = d.getSizeT(Y);
			this->tiles[x][y].postalCode = d.getSizeT(POSTAL_CODE);
			this->tiles[x][y].type = (TileType)(d.getSizeT(TYPE));
			this->tiles[x][y].cityId = d.getSizeT(CITY);
			if (d.hasValue(BUILDING)) {
				this->tiles[x][y].building = this->game->getEntityById(d.getSizeT("building"));
			}
			// Go through the save datas in the tile
			// Will be for a road/railway
			for (SaveData rd : d.getDatas()) {
				if (rd.getName() == "Road") {
					this->tiles[x][y].road = Road(rd);
				}
				else if (rd.getName() == RAILWAY) {
					this->tiles[x][y].addRailway(Railway(rd));
				}
				else if (rd.getName() == "AirplaneRoad") {
					this->tiles[x][y].airplaneRoad = AirplaneRoad(rd);
				}
			}
		}
		else if (d.getName() == POINT) {
			this->pointHeights[d.getSizeT(X)][d.getSizeT(Y)] = d.getSizeT(HEIGHT);
		}
		else if (d.getName() == TUNNEL) {
			this->tunnels.push_back(Tunnel(d, this->game));
		}
	}
}