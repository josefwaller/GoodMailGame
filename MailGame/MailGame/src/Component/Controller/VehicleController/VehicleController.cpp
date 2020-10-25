#include "VehicleController.h"

#include "Game/Game.h"
#include "System/IsoRotation/IsoRotation.h"
#include "Component/Transform/Transform.h"
#include "Component/Pathfinder/Pathfinder.h"
#include "Entity/Entity.h"
#include "System/Utils/Utils.h"
#include "System/SaveData/SaveData.h"
#include "VehicleModel/VehicleModel.h"
#include <SFML/System/Vector3.hpp>
#include <queue>
#include <map>
#include <functional>

VehicleController::VehicleController(gtime_t d, VehicleModel m) : departTime(d), stopIndex(0), pointIndex(0), model(m) {}

void VehicleController::update(float delta) {
	gtime_t travelTime = this->getEntity()->getGame()->getTime() - this->departTime;
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
	float lastStopTime = this->stops[this->stopIndex - 1].expectedTime;
	// Start at the from point
	this->points.push_back(RoutePoint(from, lastStopTime));
	// Add wait time if this is the first stop
	if (this->stopIndex == 1) {
		this->points.push_back(RoutePoint(from, lastStopTime));
	}
	auto path = this->getEntity()->pathfinder->findPathBetweenPoints(pos, to, lastStopTime, getSpeed());
	if (path.empty()) {
		// TODO
		throw std::runtime_error("Path is empty, and this is not handled");
	}
	this->points.insert(this->points.end(), path.begin(), path.end());
	// Now we can set the expected time for the stop
	this->stops[this->stopIndex].expectedTime = path.back().expectedTime;
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
		pathfindToPoint(fromStop, toStop);
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
	// This method may be called before this->getEntity() is set
	// So we set stopIndex to 0 and then have pointIndex = points.size()
	// So next tick, it will pathfind from stops[0] to stops[1]
	// And will not crash since getEntity() will be set
	this->stopIndex = 0;
	this->points = {};
	this->pointIndex = 0;
}
float VehicleController::getSpeed() {
	return VehicleModelInfo::getModelInfo(this->model).getSpeed();
}
void VehicleController::fromSaveData(SaveData data) {
	this->stopIndex = std::stoull(data.getValue("stopIndex"));
	this->departTime = std::stoull(data.getValue("departTime"));
	this->pathfindToPoint(this->stops[this->stopIndex - 1].pos, this->stops[this->stopIndex].pos);
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
