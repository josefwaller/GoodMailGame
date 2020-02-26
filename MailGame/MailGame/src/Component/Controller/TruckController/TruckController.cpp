#include "TruckController.h"
#include "System/IsoRotation/IsoRotation.h"
#include "Component/Transform/Transform.h"
#include "Component/Pathfinder/Pathfinder.h"
#include "Entity/Entity.h"
#include <SFML/System/Vector2.hpp>
#include <queue>
#include <map>
#include <functional>

void TruckController::update(float delta) {
	// If the truck has gone through all the points
	if (this->pointIndex >= this->points.size()) {
		// Arrive at destination
		// Since stopIndex starts at -1, we want to skip calling onArriveAtStop the first time
		if (this->stopIndex != -1)
			this->onArriveAtStop(this->stopIndex);
		this->goToNextStop();
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
void TruckController::pathfindToPoint(sf::Vector2f point){
	// Get the path to the point
	sf::Vector2f pos(this->getEntity()->transform->getPosition());
	this->points = this->getEntity()->pathfinder->findPathBetweenPoints(pos, point);
	// Reset pathIndex
	this->pointIndex = 0;
}
void TruckController::goToNextStop() {
	sf::Vector2f stop;
	// Go to next stop
	this->stopIndex++;
	if (this->stopIndex >= this->stops.size()) {
		// Arrive at dest
		this->onArriveAtDest();
	}
	else {
		stop = this->stops[this->stopIndex];
		// Go to the stop
		this->pathfindToPoint(stop);
	}
}
void TruckController::setStops(std::vector<sf::Vector2f> s) {
	this->stops = s;
	// -1 because it will automatically be incremented into 0
	this->stopIndex = -1;
}
void TruckController::onArriveAtTile(sf::Vector2f point) {}
void TruckController::onArriveAtStop(size_t stopIndex) {}
void TruckController::onArriveAtDest() {}
