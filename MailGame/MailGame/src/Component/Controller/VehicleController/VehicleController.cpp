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
			VehicleControllerStop lastPoint = this->points[this->pointIndex - 1];
			VehicleControllerStop nextPoint = this->points[this->pointIndex];
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
void VehicleController::pathfindToPoint(sf::Vector3f from, sf::Vector3f to){
	// Get the path to the point
	sf::Vector3f pos(from);
	this->points = {};
	sf::Vector3f lastPoint = pos;
	float totalDistance = this->stops[this->stopIndex - 1].expectedTime;
	// Start at the from point
	this->points.push_back(VehicleControllerStop(from, totalDistance));
	for (auto s : this->getEntity()->pathfinder->findPathBetweenPoints(pos, to)) {
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
	this->stops = {};
	for (auto s : stops) {
		// Set the expected time to 0 for now, it will be overridden when going to that stop
		this->stops.push_back(VehicleControllerStop(s, 0.0f));
	}
	this->stopIndex = 0;
}
float VehicleController::getPathDistance(sf::Vector3f from, sf::Vector3f to) {
	float toReturn = 0.0f;
	std::vector<sf::Vector3f> points = this->getEntity()->pathfinder->findPathBetweenPoints(from, to);
	sf::Vector3f lastPoint = points[0];
	for (sf::Vector3f point : points) {
		toReturn += Utils::getVectorDistance(point, lastPoint);
		lastPoint = point;
	}
	return toReturn;
}
void VehicleController::fromSaveData(SaveData data) {
	this->stopIndex = std::stoull(data.getValue("stopIndex"));
	this->departTime = std::stoull(data.getValue("departTime"));
	// Set the expected arrival time for each stop up to the one it's going to
	float totalDistance = 0.0f;
	for (size_t i = 1; i < this->stopIndex; i++) {
		totalDistance += getPathDistance(this->stops[i - 1].pos, this->stops[i].pos) * Game::UNITS_IN_GAME_HOUR * getSpeed();
		this->stops[i].expectedTime = totalDistance;
	}
	// Generate the points from the last stop to the next stop
	if (stopIndex < 1) {
		throw std::runtime_error("Invalid stopIndex, must be more than 0");
	}
	this->pathfindToPoint(this->stops[this->stopIndex - 1].pos, this->stops[this->stopIndex].pos);
}
std::optional<SaveData> VehicleController::getSaveData() {
	SaveData data("VehicleController");
	data.addValue("stopIndex", stopIndex);
	data.addValue("departTime", departTime);
	return { data };
}
std::vector<sf::Vector3f> VehicleController::getArrivingTransitPath(std::shared_ptr<Entity> e, TransitStop::TransitType type) {
	auto transit = e->transitStop;
	switch (type) {
	case TransitStop::TransitType::Car:
		return { transit->getCarStop().tile };
	case TransitStop::TransitType::Train:
		return { transit->getTrainStop().tile };
	case TransitStop::TransitType::Airplane:
		// Compute where we need to go
		auto aStop = transit->getAirplaneStop();
		auto dir = aStop.end - aStop.begin;
		// TBA add z
		sf::Vector3f unit = dir / (sqrt(dir.x * dir.x + dir.y * dir.y));
		// Th horizontal distance to cover when descending
		const float DESCENT_LENGTH = 5.0f;
		const float PLANE_HEIGHT = 4.0f;
		sf::Vector3f airStart = aStop.begin - DESCENT_LENGTH * unit;
		airStart.z = PLANE_HEIGHT;
		sf::Vector3f airEnd = aStop.end + unit * DESCENT_LENGTH;
		airEnd.z = PLANE_HEIGHT;
		return  { airStart, aStop.begin, aStop.end };
	}
	return {};
}
std::vector<sf::Vector3f> VehicleController::getDepartingTransitPath(std::shared_ptr<Entity> e, TransitStop::TransitType type) {
	auto transit = e->transitStop;
	// TODO: Once there are better depot types/more defined railways and driveways, this will be more complicated
	switch (type) {
	case TransitStop::TransitType::Car:
	case TransitStop::TransitType::Train:
		// This is a hacky temporary shortcut
		return getArrivingTransitPath(e, type);
	case TransitStop::TransitType::Airplane:
		// Compute where we need to go
		auto aStop = transit->getAirplaneStop();
		auto dir = aStop.end - aStop.begin;
		// TBA add z
		sf::Vector3f unit = dir / (sqrt(dir.x * dir.x + dir.y * dir.y));
		// Th horizontal distance to cover when descending
		const float DESCENT_LENGTH = 5.0f;
		const float PLANE_HEIGHT = 4.0f;
		sf::Vector3f airStart = aStop.begin - DESCENT_LENGTH * unit;
		airStart.z = PLANE_HEIGHT;
		sf::Vector3f airEnd = aStop.end + unit * DESCENT_LENGTH;
		airEnd.z = PLANE_HEIGHT;
		return  { aStop.begin, aStop.end, airEnd };
	}
}
void VehicleController::onArriveAtTile(sf::Vector2i point) {}
void VehicleController::onArriveAtStop(size_t stopIndex) {}
void VehicleController::onArriveAtDest() {}
