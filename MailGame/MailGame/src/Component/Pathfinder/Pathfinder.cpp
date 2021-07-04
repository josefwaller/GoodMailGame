#include "Pathfinder.h"
#include "System/Utils/Utils.h"
#include "Game/Game.h"
#include <queue>

gtime_t Pathfinder::getTimeBetween(sf::Vector3f one, sf::Vector3f two, float speed) {
	return (gtime_t)((float)Game::UNITS_IN_GAME_HOUR * (Utils::getVectorDistance(one, two) / speed));
}

std::vector<sf::Vector2i> Pathfinder::findBoatPath(GameMap* gMap, sf::Vector2i start, sf::Vector2i end) {
	std::queue<sf::Vector2i> potentialTiles;
	std::vector<sf::Vector2i> visitedTiles;

	auto previous = std::map<sf::Vector2i, sf::Vector2i, std::function<bool(sf::Vector2i one, sf::Vector2i two)>>{
		[](sf::Vector2i one, sf::Vector2i two) {
			return 1000 * one.x + one.y > 1000 * two.x + two.y;
		}
	};
	potentialTiles.push(start);
	while (!potentialTiles.empty()) {
		sf::Vector2i t = potentialTiles.front();
		potentialTiles.pop();
		// Check we haven't been here before
		if (std::find(visitedTiles.begin(), visitedTiles.end(), t) != visitedTiles.end()) {
			continue;
		}
		visitedTiles.push_back(t);
		// Check if it's end
		if (t == end) {
			std::vector<sf::Vector2i> toReturn = { t };
			while (t != start) {
				t = previous.at(t);
				toReturn.push_back(t);
			}
			std::reverse(toReturn.begin(), toReturn.end());
			return toReturn;
		}
		// Check it's underwater
		size_t n = 0;
		for (size_t i = 0; i < 2; i++) {
			for (size_t j = 0; j < 2; j++) {
				if (gMap->getPointHeight(t.x + i, t.y + j) < GameMap::SEA_LEVEL) {
					n++;
				}
			}
		}
		if (n < 2) {
			if (t == sf::Vector2i(9, 3))
				auto x = 0;
			continue;
		}
		// Add ajacent tiles
		for (int i = -1; i < 2; i++) {
			for (int j = -1; j < 2; j++) {
				if (t == sf::Vector2i(9, 3))
					auto x = 0;
				if ((i == 0 || j == 0) && i != j) {
					sf::Vector2i toAdd = t + sf::Vector2i(i, j);
					if (gMap->getTileAt(toAdd).type != TileType::OffMap) {
						potentialTiles.push(toAdd);
						previous.insert({ toAdd, t });
					}
				}
			}
		}
	}
	return {};
}

std::vector<sf::Vector2i> Pathfinder::findCarPath(GameMap* gMap, sf::Vector2i start, sf::Vector2i end) {
	// Need to pathfind from current position to the destination
	// Find a path along only road from pos to stop
	std::queue<sf::Vector2i> potentialPoints;
	std::vector<sf::Vector2i> visitedPoints;
	//
	auto previous = std::map<sf::Vector2i, sf::Vector2i, std::function<bool(sf::Vector2i one, sf::Vector2i two)>>{
		[](sf::Vector2i one, sf::Vector2i two) {
			return 1000 * one.x + one.y > 1000 * two.x + two.y;
		}
	};
	potentialPoints.push(start);
	visitedPoints.push_back(start);
	while (!potentialPoints.empty()) {
		// Get the first point
		sf::Vector2i point = potentialPoints.front();
		potentialPoints.pop();
		// Ensure it has a road
		Tile fromTile = gMap->getTileAt(point.x, point.y);
		if (!fromTile.road.has_value()) {
			continue;
		}
		// Check if it is the correct one
		if (point == end) {
			// Gather points along the path
			std::vector<sf::Vector2i> pathPoints;
			while (point != start) {
				pathPoints.push_back(point);
				point = previous[point];
			}
			// Add start as well
			pathPoints.push_back(start);
			std::reverse(pathPoints.begin(), pathPoints.end());
			return pathPoints;
		}
		// Add the points around it
		std::vector<sf::Vector2i> toAdd = fromTile.road.value().getConnectedTiles(point);
		// Add any points to potential points we've not visited
		for (auto it = toAdd.begin(); it != toAdd.end(); it++) {
			if (std::find(visitedPoints.begin(), visitedPoints.end(), *it) == visitedPoints.end()) {
				potentialPoints.push(*it);
				visitedPoints.push_back(*it);
				previous[*it] = point;
			}
		}
	}
	throw std::runtime_error("Cannot find a path");
}