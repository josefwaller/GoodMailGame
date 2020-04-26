#include "VehicleController.h"
#include "System/IsoRotation/IsoRotation.h"
#include "Component/Transform/Transform.h"
#include "Component/Pathfinder/Pathfinder.h"
#include "Entity/Entity.h"
#include <SFML/System/Vector3.hpp>
#include <queue>
#include <map>
#include <functional>

void VehicleController::update(float delta) {
	// If the truck has gone through all the points
	// Also called if the truck has no points, i.e. no path exists
	if (this->pointIndex >= this->points.size()) {
		// Arrive at destination
		// Since stopIndex starts at -1, we want to skip calling onArriveAtStop the first time
		if (this->stopIndex != -1)
			this->onArriveAtStop(this->stopIndex);
		this->goToNextStop();
	}
	else {
		// Check if the truck is close enough to the point
		sf::Vector3f point(this->points[this->pointIndex]);
		auto trans = this->getEntity()->transform;
		sf::Vector3f pos = trans->getPosition();
		if (pow(pos.x - point.x, 2) + pow(pos.y - point.y, 2) <= pow(0.1f, 2)) {
			// Go to the next point
			this->pointIndex++;
			// Set position to point to avoid being a bit off
			trans->setPosition(point);
			// Call callback
			this->onArriveAtTile(sf::Vector2i(point.x, point.y));
		}
		else {
			// Move closer to stop
			sf::Vector3f diff = point - pos;
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
void VehicleController::pathfindToPoint(sf::Vector3f point){
	// Get the path to the point
	sf::Vector3f pos(this->getEntity()->transform->getPosition());
	this->points = this->getEntity()->pathfinder->findPathBetweenPoints(pos, point);
	// Reset pathIndex
	this->pointIndex = 0;
}
void VehicleController::goToNextStop() {
	sf::Vector3f stop;
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
void VehicleController::setStops(std::vector<sf::Vector3f> s) {
	this->stops = s;
	// -1 because it will automatically be incremented into 0
	this->stopIndex = -1;
}
void VehicleController::onArriveAtTile(sf::Vector2i point) {}
void VehicleController::onArriveAtStop(size_t stopIndex) {}
void VehicleController::onArriveAtDest() {}
