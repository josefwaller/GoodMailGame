#include "VehicleController.h"

#include "Game/Game.h"
#include "System/IsoRotation/IsoRotation.h"
#include "Component/Transform/Transform.h"
#include "Component/Pathfinder/Pathfinder.h"
#include "Entity/Entity.h"
#include "System/Utils/Utils.h"
#include <SFML/System/Vector3.hpp>
#include <queue>
#include <map>
#include <functional>

VehicleController::VehicleController(gtime_t d) : departTime(d) {}

void VehicleController::update(float delta) {
	gtime_t travelTime = this->getEntity()->getGame()->getTime() - this->departTime;
	/*
		// Arrive at destination
		// Since stopIndex starts at -1, we want to skip calling onArriveAtStop the first time
		if (this->stopIndex != -1)
			this->onArriveAtStop(this->stopIndex);
		this->goToNextStop();
*/
	if (this->pointIndex >= this->points.size()) {
		// Go to next stop
		this->goToNextStop();
	}
	// If the truck has gone through all the points
	// Also called if the truck has no points, i.e. no path exists
	if (travelTime >= this->points[this->pointIndex].expectedTime) {
		sf::Vector3f point = this->points[this->pointIndex].pos;
		// Go to the next point
		this->pointIndex++;
		// Call callback
		if (this->pointIndex >= this->points.size()) {
			this->onArriveAtStop(this->stopIndex);
			this->goToNextStop();
		}
		else {
			this->onArriveAtTile(sf::Vector2i(point.x, point.y));
		}
	}
	else {
		VehicleControllerStop lastPoint = this->points[this->pointIndex - 1];
		VehicleControllerStop nextPoint = this->points[this->pointIndex];
		gtime_t timeSincePoint = travelTime - lastPoint.expectedTime;
		sf::Vector3f pos = lastPoint.pos + (nextPoint.pos - lastPoint.pos) * (float)(timeSincePoint / (float)(nextPoint.expectedTime - lastPoint.expectedTime));
		this->getEntity()->transform->setPosition(pos);
	}
}
void VehicleController::pathfindToPoint(sf::Vector3f from, sf::Vector3f point){
	// Get the path to the point
	sf::Vector3f pos(from);
	this->points = {};
	sf::Vector3f lastPoint = pos;
	float totalDistance = this->stops[this->stopIndex - 1].expectedTime;
	for (auto s : this->getEntity()->pathfinder->findPathBetweenPoints(pos, point)) {
		totalDistance += Utils::getVectorDistance(lastPoint, s) * Game::UNITS_IN_GAME_HOUR * getSpeed();
		// Currently, just assume units go 1 tile per second
		// TODO: Change this to actual speed
		this->points.push_back(VehicleControllerStop(s, (gtime_t)totalDistance));
		lastPoint = s;
	}
	// Now we can set the expected time for the stop
	this->stops[this->stopIndex].expectedTime = totalDistance;
	// Reset pathIndex
	this->pointIndex = 1;
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
		sf::Vector3f fromStop = this->stops[this->stopIndex - 1].pos;
		sf::Vector3f toStop = this->stops[this->stopIndex].pos;
		// Go to the stop
		this->pathfindToPoint(fromStop, toStop);
	}
}
void VehicleController::setStops(std::vector<sf::Vector3f> stops) {
	if (stops.size() < 2) {
		throw std::runtime_error("Route with less than two stops created!");
	}
	this->stops = {};
	for (auto s : stops) {
		// Set the expected time to 0 for now, it will be overridden when going to that stop
		this->stops.push_back(VehicleControllerStop(s, 0.0f));
	}
	this->stopIndex = 0;
}
void VehicleController::onArriveAtTile(sf::Vector2i point) {}
void VehicleController::onArriveAtStop(size_t stopIndex) {}
void VehicleController::onArriveAtDest() {}
