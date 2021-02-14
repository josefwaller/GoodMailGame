#include "RoadPathfinder.h"
#include <queue>
#include "Entity/Entity.h"
#include "Game/Game.h"
#include "GameMap/GameMap.h"

std::vector<SpeedPoint> RoadPathfinder::findPathBetweenPoints(
	sf::Vector3f from,
	sf::Vector3f to,
	gtime_t departTime,
	float speed) {
	// Get Gamemap
	GameMap* gMap = this->getEntity()->getGame()->getGameMap();
	// Need to pathfind from current position to the destination
	// Find a path along only road from pos to stop
	std::queue<sf::Vector3i> potentialPoints;
	std::vector<sf::Vector3i> visitedPoints;
	//
	auto previous = std::map<sf::Vector3i, sf::Vector3i, std::function<bool(sf::Vector3i one, sf::Vector3i two)>>{
		[](sf::Vector3i one, sf::Vector3i two) {
			return 1000 * one.x + one.y > 1000 * two.x + two.y;
		}
	};
	potentialPoints.push(sf::Vector3i(from));
	visitedPoints.push_back(sf::Vector3i(from));
	while (!potentialPoints.empty()) {
		// Get the first point
		sf::Vector3i point = potentialPoints.front();
		potentialPoints.pop();
		// Ensure it has a road
		Tile fromTile = gMap->getTileAt(point.x, point.y);
		if (!fromTile.road.has_value()) {
			throw std::runtime_error("Pathinging to/from point that does not has a road");
		}
		// Check if it is the correct one
		if (point == sf::Vector3i(to)) {
			// Gather points along the path
			std::vector<sf::Vector3f> pathPoints;
			while (point != sf::Vector3i(from)) {
				pathPoints.push_back(sf::Vector3f(point));
				point = previous[point];
			}
			std::reverse(pathPoints.begin(), pathPoints.end());
			std::vector<SpeedPoint> toReturn = { from };
			for (auto it = pathPoints.begin(); it != pathPoints.end(); it++) {
				toReturn.push_back(SpeedPoint(*it));
			}
			return toReturn;
		}
		// Add the points around it
		std::vector<sf::Vector3i> toAdd;
		for (size_t i = 0; i < 4; i++) {
			IsoRotation rot(i);
			sf::Vector3i off = point + sf::Vector3i(rot.getUnitVector3D());
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