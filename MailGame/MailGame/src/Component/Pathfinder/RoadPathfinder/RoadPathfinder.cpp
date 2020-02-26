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
		for (int x = -1; x < 2; x++) {
			for (int y = -1; y < 2; y++) {
				if ((x == 0 || y == 0) && x != y) {
					sf::Vector2i p(point.x + x, point.y + y);
					if (gMap->getTileAt(p.x, p.y).type == TileType::Road
						&& std::find(visitedPoints.begin(), visitedPoints.end(), p) == visitedPoints.end()) {
						// Add the point
						potentialPoints.push(p);
						visitedPoints.push_back(p);
						previous[p] = point;
					}
				}
			}
		}
	}
	throw std::runtime_error("Cannot find a path");
}
