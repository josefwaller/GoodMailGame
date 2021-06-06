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
}
void GameMap::generateNew() {
	FastNoiseLite noise(18);
	noise.SetNoiseType(FastNoiseLite::NoiseType::NoiseType_Perlin);
	noise.SetFrequency(0.01f);
	noise.SetFractalType(FastNoiseLite::FractalType::FractalType_FBm);
	noise.SetFractalOctaves(5);
	noise.SetFractalLacunarity(2.0f);
	noise.SetFractalGain(0.5f);

	// Set up point heights
	this->pointHeights = std::vector<std::vector<unsigned int>>(MAP_WIDTH + 1, std::vector<unsigned int>(MAP_HEIGHT + 1, 0.0f));
	for (size_t i = 0; i < MAP_WIDTH + 1; i++) {
		for (size_t j = 0; j < MAP_HEIGHT + 1; j++) {
			float n = noise.GetNoise((float)i, (float)j);
			unsigned int val = (unsigned int)abs(round(10.0f * n));
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
}

sf::Sprite GameMap::getRoadSprite(Road road, IsoRotation gameRotation, std::vector<std::vector<unsigned int>> heights) {
	std::string spriteString = "road-";
	if (IsoRotation(IsoRotation::NORTH) < gameRotation) {
	}
	std::map<IsoRotation, bool> hasDirection;
	for (IsoRotation r : road.getDirections()) {
		hasDirection[r + gameRotation] = true;
	}
	for (IsoRotation r : IsoRotation::CARDINAL_DIRECTIONS) {
		spriteString += hasDirection[r] ? "1" : "0";
	}
	if (heights[0][0] != heights[0][1] || heights[0][0] != heights[1][0]) {
		// Must be a ramp
	//	if (heights[0][0] > heights[0][1]) {
	//	}
	}
	return ResourceLoader::get()->getSprite("tiles/tiles", spriteString);
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
			/*	e->renderer->render(window);
				return;*/
		}
	}
	sf::Sprite s;
	Tile tile = tiles[x][y];
	switch (tile.type) {
	case TileType::Land:
		if (tile.road.has_value()) {
			s = getRoadSprite(tile.road.value(), this->game->getRotation());
		}
		else if (tile.airplaneRoad.has_value()) {
			s = getRoadSprite(tile.airplaneRoad.value(), this->game->getRotation());
			s.setColor(tile.airplaneRoad.value().isRunway ? sf::Color::Blue : sf::Color::Red);
		}
		else if (!tile.getRailways().empty()) {
			s = EMPTY_SPRITE;
		}
		else {
			sf::Color tileColor = (x + y) % 2 == 0 ? sf::Color(0, 150, 0) : sf::Color(0, 175, 0);
			sf::VertexArray arr(sf::PrimitiveType::Quads, 4);
			arr[0] = sf::Vertex(this->game->worldToScreenPos(sf::Vector3f(x, y, this->pointHeights.at(x).at(y))), tileColor);
			arr[1] = sf::Vertex(this->game->worldToScreenPos(sf::Vector3f(x + 1, y, this->pointHeights.at(x + 1).at(y))), tileColor);
			arr[2] = sf::Vertex(this->game->worldToScreenPos(sf::Vector3f(x + 1, y + 1, this->pointHeights.at(x + 1).at(y + 1))), tileColor);
			arr[3] = sf::Vertex(this->game->worldToScreenPos(sf::Vector3f(x, y + 1, this->pointHeights.at(x).at(y + 1))), tileColor);
			window->draw(arr);
			return;
		}
	}
	unsigned int height = 100000;
	for (size_t xOff = 0; xOff < 2; xOff++) {
		for (size_t yOff = 0; yOff < 2; yOff++) {
			if (height > this->pointHeights.at(x + xOff).at(y + yOff)) {
				height = this->pointHeights.at(x + xOff).at(y + yOff);
			}
		}
	}
	sf::Vector3f pos((float)x + 0.5f, (float)y + 0.5f, (float)height);
	s.setPosition(this->game->worldToScreenPos(pos));
	s = Utils::setupBuildingSprite(s, false);
	if (!tile.canGetLock()) {
		s.setColor(sf::Color(0, 255, 51));
	}
	window->draw(s);
	// Draw the railway if it has one
	if (!tile.getRailways().empty()) {
		for (Railway r : tile.getRailways()) {
			// Point is center of tile + the direction
			sf::Vector2f center = sf::Vector2f(x, y) + sf::Vector2f(0.5, 0.5);
			sf::Vector2f fromPoint = center + 0.5f * r.getFrom().getUnitVector();
			sf::Vector2f toPoint = center + 0.5f * r.getTo().getUnitVector();
			// Render line
			sf::VertexArray arr(sf::PrimitiveType::Lines, 2);
			arr[0] = sf::Vertex(this->game->worldToScreenPos(Utils::toVector3f(fromPoint)), r.getIsLocked() ? sf::Color::White : sf::Color::Black);
			arr[1] = sf::Vertex(this->game->worldToScreenPos(Utils::toVector3f(toPoint)), r.isStation ? sf::Color::Blue : sf::Color::Red);
			window->draw(arr);
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
					// Make sure tile is valid for road
					for (size_t y = 0; y < 2; y++) {
						unsigned int height = std::min(this->pointHeights.at(pos.x).at(pos.y + y), this->pointHeights.at(pos.x + 1).at(pos.y + y));
						this->pointHeights.at(pos.x).at(pos.y + y) = height;
						this->pointHeights.at(pos.x + 1).at(pos.y + y) = height;
					}
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
					// Make sure tile is valid for road
					for (size_t x = 0; x < 2; x++) {
						unsigned int height = std::min(this->pointHeights.at(pos.x + x).at(pos.y), this->pointHeights.at(pos.x + x).at(pos.y + 1));
						this->pointHeights.at(pos.x + x).at(pos.y) = height;
						this->pointHeights.at(pos.x + x).at(pos.y + 1) = height;
					}
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
							switch (xOff) {
							case -1:
								rot = IsoRotation::EAST;
								break;
							case 1:
								rot = IsoRotation::WEST;
								break;
							case 0:
								rot = yOff == 1 ? IsoRotation::NORTH : IsoRotation::SOUTH;
							}
							std::shared_ptr<Entity> e = BuildingPresets::residence(this->game, sf::Vector3f((float)(x + xOff), (float)(y + yOff), 0), rot);
							this->game->addEntity(
								e
							);
							this->tiles[x + xOff][y + yOff].building = e;
							this->tiles[x + xOff][y + yOff].cityId = cityId;
						}
					}
				}
			}
		}
	}
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
	std::vector<sf::Vector2i> toReturn;
	for (size_t x = 0; x < MAP_WIDTH; x++) {
		for (size_t y = 0; y < MAP_HEIGHT; y++) {
			Tile t = this->getTileAt(x, y);
			if (t.building.lock() && t.building.lock()->tag == EntityTag::Residence) {
				toReturn.push_back(sf::Vector2i(x, y));
			}
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
	return sd;
}
void GameMap::loadFromSaveData(SaveData data) {
	using namespace SaveKeys;
	// Go through every tile data
	for (SaveData d : data.getDatas()) {
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
}