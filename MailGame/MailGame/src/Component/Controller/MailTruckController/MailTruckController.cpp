#include "MailTruckController.h"
#include "Game/Game.h"
#include <stdexcept>
#include "Entity/Entity.h"
#include "Component/Transform/Transform.h"
#include "GameMap/GameMap.h"
#include <cmath>
#include <queue>

const float MailTruckController::SPEED = 1.0f;

MailTruckController::MailTruckController(MailTruckRoute r): route(r), pointIndex(0), stopIndex(0) {
}

void MailTruckController::update(float delta) {
	// If the truck has gone through all the points
	if (this->pointIndex >= this->points.size()) {
		// Go to the next stop
		stopIndex = (stopIndex + 1) % this->route.stops.size();
		pathfindToNextStop();
	}
	else {
		// Check if the truck is close enough to the point
		sf::Vector2f point(this->points[this->pointIndex]);
		auto trans = this->getEntity()->transform;
		sf::Vector2f pos = trans->getPosition();
		if (pow(pos.x - point.x, 2) + pow(pos.y - point.y, 2) <= pow(0.1f, 2)) {
			// Go to the next point
			this->pointIndex++;
		}
		else {
			// Move closer to stop
			sf::Vector2f diff = point - pos;
			float magDiff = sqrt(pow(diff.x, 2) + pow(diff.y, 2));
			trans->setPosition(trans->getPosition() + (diff / magDiff) * SPEED * delta);
		}
	}
}
void MailTruckController::pathfindToNextStop() {
	// Need to pathfind from current position to the stop
	GameMap* gMap = this->getEntity()->getGame()->getGameMap();
	sf::Vector2i pos(this->getEntity()->transform->getPosition());
	sf::Vector2i stop = this->route.stops[stopIndex].target.value();
	// Find a path along only road from pos to stop
	std::queue<sf::Vector2i> potentialPoints;
	std::vector<sf::Vector2i> visitedPoints;
	//
	auto previous = std::map<sf::Vector2i, sf::Vector2i, std::function<bool(sf::Vector2i one, sf::Vector2i two)>>{
		[](sf::Vector2i one, sf::Vector2i two) {
			return 1000 * one.x + one.y > 1000 * two.x + two.y;
		}
	};
	potentialPoints.push(pos);
	visitedPoints.push_back(pos);
	while (!potentialPoints.empty()) {
		// Get the first point
		sf::Vector2i point = potentialPoints.front();
		potentialPoints.pop();
		// Check if it is the correct one
		if (point == stop) {
			// Set points
			this->points.clear();
			while (point != pos) {
				this->points.push_back(point);
				point = previous[point];
			}
			std::reverse(this->points.begin(), this->points.end());
			this->pointIndex = 0;
			return;
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