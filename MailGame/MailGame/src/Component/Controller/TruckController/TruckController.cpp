#include "TruckController.h"
#include "System/IsoRotation/IsoRotation.h"
#include "Component/Transform/Transform.h"
#include "Entity/Entity.h"
#include "GameMap/GameMap.h"
#include "Game/Game.h"
#include <SFML/System/Vector2.hpp>
#include <queue>
#include <map>
#include <functional>

void TruckController::update(float delta) {
	// If the truck has gone through all the points
	if (this->pointIndex >= this->points.size()) {
		// Arrive at destination
		this->onArriveAtDest();
	}
	else {
		// Check if the truck is close enough to the point
		sf::Vector2f point(this->points[this->pointIndex]);
		auto trans = this->getEntity()->transform;
		sf::Vector2f pos = trans->getPosition();
		if (pow(pos.x - point.x, 2) + pow(pos.y - point.y, 2) <= pow(0.1f, 2)) {
			// Go to the next point
			this->pointIndex++;
			// Set position to point to avoid being a bit off
			trans->setPosition(point);
			// Call callback
			this->onArriveAtTile(point);
		}
		else {
			// Move closer to stop
			sf::Vector2f diff = point - pos;
			float magDiff = sqrt(pow(diff.x, 2) + pow(diff.y, 2));
			trans->setPosition(trans->getPosition() + (diff / magDiff) * getSpeed() * delta);
			// Set rotation accordingly
			if (diff.x < 0.01 && diff.x > -0.01) {
				if (diff.y < 0) {
					trans->setRotation(IsoRotation::NORTH);
				}
				else {
					trans->setRotation(IsoRotation::SOUTH);
				}
			} else {
				if (diff.x < 0) {
					trans->setRotation(IsoRotation::WEST);
				}
				else {
					trans->setRotation(IsoRotation::EAST);
				}
			}
		}
	}
}
void TruckController::pathfindToPoint(sf::Vector2i point){
	// Get the path to the point
	sf::Vector2i pos(this->getEntity()->transform->getPosition());
	this->points = this->getPathBetween(pos, point, this->getEntity()->getGame()->getGameMap());
	// Reset pathIndex
	this->pointIndex = 0;
}
std::vector<sf::Vector2i> TruckController::getPathBetween(sf::Vector2i from, sf::Vector2i to, GameMap* gMap) {
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
	potentialPoints.push(from);
	visitedPoints.push_back(from);
	while (!potentialPoints.empty()) {
		// Get the first point
		sf::Vector2i point = potentialPoints.front();
		potentialPoints.pop();
		// Check if it is the correct one
		if (point == to) {
			// Gather points along the path
			std::vector<sf::Vector2i> pathPoints;
			while (point != from) {
				pathPoints.push_back(point);
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

void TruckController::onArriveAtTile(sf::Vector2f point) {}
