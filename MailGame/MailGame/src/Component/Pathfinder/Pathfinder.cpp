#include "Pathfinder.h"
#include "System/Utils/Utils.h"
#include "Game/Game.h"
#include <queue>

gtime_t Pathfinder::getTimeBetween(sf::Vector3f one, sf::Vector3f two, float speed) {
	return (gtime_t)((float)Game::UNITS_IN_GAME_HOUR * (Utils::getVectorDistance(one, two) / speed));
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