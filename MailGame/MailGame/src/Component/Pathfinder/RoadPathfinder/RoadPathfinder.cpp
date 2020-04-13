#include "RoadPathfinder.h"
#include <queue>
#include "Entity/Entity.h"
#include "Game/Game.h"
#include "GameMap/GameMap.h"

std::vector<sf::Vector2f> RoadPathfinder::findPathBetweenPoints(sf::Vector2f from, sf::Vector2f to) {
	// Get Gamemap
	GameMap* gMap = this->getEntity()->getGame()->getGameMap();
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
	potentialPoints.push(sf::Vector2i(from));
	visitedPoints.push_back(sf::Vector2i(from));
	while (!potentialPoints.empty()) {
		// Get the first point
		sf::Vector2i point = potentialPoints.front();
		potentialPoints.pop();
		// Ensure it has a road
		Tile fromTile = gMap->getTileAt(point.x, point.y);
		if (!fromTile.road.has_value()) {
			throw std::runtime_error("Pathinging to/from point that does not has a road");
		}
		// Check if it is the correct one
		if (point == sf::Vector2i(to)) {
			// Gather points along the path
			std::vector<sf::Vector2f> pathPoints;
			while (point != sf::Vector2i(from)) {
				pathPoints.push_back(sf::Vector2f(point));
				point = previous[point];
			}
			std::reverse(pathPoints.begin(), pathPoints.end());
			return pathPoints;
		}
		// Add the points around it
		std::vector<sf::Vector2i> toAdd;
		for (size_t i = 0; i < 4; i++) {
			IsoRotation rot(i);
			sf::Vector2i off = point + sf::Vector2i(rot.getUnitVector());
			if (gMap->hasRoadInDirection(point.x, point.y, rot) && gMap->hasRoadInDirection(off.x, off.y, rot + 2)) {
				toAdd.push_back(off);
			}
		}
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
