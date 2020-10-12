#include "VehicleController.h"

#include "Game/Game.h"
#include "System/IsoRotation/IsoRotation.h"
#include "Component/Transform/Transform.h"
#include "Component/Pathfinder/Pathfinder.h"
#include "Entity/Entity.h"
#include "System/Utils/Utils.h"
#include "System/SaveData/SaveData.h"
#include <SFML/System/Vector3.hpp>
#include <queue>
#include <map>
#include <functional>

VehicleController::VehicleController(gtime_t d) : departTime(d), stopIndex(0), pointIndex(0) {}

void VehicleController::update(float delta) {
	gtime_t travelTime = this->getEntity()->getGame()->getTime();// -this->departTime;
	if (this->pointIndex >= this->points.size()) {
		// Go to next stop
		this->goToNextStop();
	}
	else {
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
			RoutePoint lastPoint = this->points[this->pointIndex - 1];
			RoutePoint nextPoint = this->points[this->pointIndex];
			gtime_t timeSincePoint = travelTime - lastPoint.expectedTime;
			sf::Vector3f pos = lastPoint.pos + (nextPoint.pos - lastPoint.pos) * (float)(timeSincePoint / (float)(nextPoint.expectedTime - lastPoint.expectedTime));
			this->getEntity()->transform->setPosition(pos);
			// Get rotation
			sf::Vector2f diff(nextPoint.pos.x - lastPoint.pos.x, nextPoint.pos.y - lastPoint.pos.y);
			IsoRotation rot = IsoRotation::fromUnitVector(diff);
			this->getEntity()->transform->setRotation(rot);
		}
	}
}
// This function will eventually be used if the vehicle's route is altered and it cannot complete it's original path
// For now it is not used
void VehicleController::pathfindToPoint(sf::Vector3f from, sf::Vector3f to){
	// Get the path to the point
	sf::Vector3f pos(from);
	this->points = {};
	sf::Vector3f lastPoint = pos;
	float totalDistance = this->stops[this->stopIndex - 1].expectedTime;
	// Start at the from point
	this->points.push_back(RoutePoint(from, totalDistance));
	// Add wait time if this is the first stop
	if (this->stopIndex == 1) {
		this->points.push_back(RoutePoint(from, totalDistance));
	}
	for (auto s : this->getEntity()->pathfinder->findPathBetweenPoints(pos, to, totalDistance, 1.0f)) {
		totalDistance += Utils::getVectorDistance(lastPoint, s.pos) * Game::UNITS_IN_GAME_HOUR * getSpeed();
		// Currently, just assume units go 1 tile per second
		// TODO: Change this to actual speed
		this->points.push_back(RoutePoint(s.pos, (gtime_t)totalDistance));
		lastPoint = s.pos;
	}
	this->points.push_back(RoutePoint(to, totalDistance));
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
		this->points = this->stops[this->stopIndex].points;
		this->pointIndex = 0;
	}
}
float VehicleController::getPathDistance(sf::Vector3f from, sf::Vector3f to) {
	float toReturn = 0.0f;
	std::vector<sf::Vector3f> points;// = this->getEntity()->pathfinder->findPathBetweenPoints(from, to);
	// If points is empty, just return 0
	if (points.size() == 0)
		return toReturn;
	sf::Vector3f lastPoint = points[0];
	for (sf::Vector3f point : points) {
		toReturn += Utils::getVectorDistance(point, lastPoint);
		lastPoint = point;
	}
	return toReturn;
	return 0.0f;
}
void VehicleController::setStops(std::vector<VehicleControllerStop> stops) {
	this->stops = stops;
	this->points = stops.empty() ? std::vector<RoutePoint>() : stops.front().points;
}
void VehicleController::fromSaveData(SaveData data) {
	this->stopIndex = std::stoull(data.getValue("stopIndex"));
	this->pointIndex = std::stoull(data.getValue("pointIndex"));
	this->points = this->stops[this->stopIndex].points;
	this->departTime = std::stoull(data.getValue("departTime"));
}
std::optional<SaveData> VehicleController::getSaveData() {
	SaveData data("VehicleController");
	data.addValue("stopIndex", stopIndex);
	data.addValue("pointIndex", pointIndex);
	data.addValue("departTime", departTime);
	return { data };
}

void VehicleController::onArriveAtTile(sf::Vector2i point) {}
void VehicleController::onArriveAtStop(size_t stopIndex) {}
void VehicleController::onArriveAtDest() {}
