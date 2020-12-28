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
#include <math.h>

VehicleController::VehicleController(gtime_t d, VehicleModel m, std::vector<std::weak_ptr<Entity>> cargoCars) : departTime(d), stopIndex(0), pointIndex(0), model(m), cargoCars(cargoCars) {}

void VehicleController::update(float delta) {
	gtime_t travelTime = this->getEntity()->getGame()->getTime();
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
				// Now we know when we arrive at that stop
				this->stops[this->stopIndex].expectedTime = this->points.back().expectedTime;
				this->stops[this->stopIndex].distance = this->points.back().distance;
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
			// Update cars
			const float BETWEEN_CARS_DISTANCE = 0.3f;
			float currentDistance = Utils::getVectorDistance(pos, lastPoint.pos) + lastPoint.distance;
			for (std::weak_ptr<Entity> car : this->cargoCars) {
				if (car.lock()) {
					currentDistance -= BETWEEN_CARS_DISTANCE;
					if (currentDistance >= 0) {
						auto pair = getPosAndRotAtDistance(currentDistance);
						car.lock()->transform->setPosition(pair.first);
						car.lock()->transform->setRotation(pair.second);
					}
				}
			}
		}
	}
}

std::vector<RoutePoint> VehicleController::getPathBetweenStops(VehicleControllerStop from, VehicleControllerStop to) {
	std::vector<RoutePoint> points;
	// First, it always starts at the first point in the departing path when departing from
	points.push_back(RoutePoint(from.departingPath.front().getPos(), from.expectedTime, from.distance));
	gtime_t departTime = from.expectedTime;
	// Add the departing path, the vehicle departs after waiting
	auto path = Utils::speedPointVectorToRoutePointVector(from.departingPath, departTime + from.waitTime, getSpeed());
	points.insert(points.end(), path.begin(), path.end());
	departTime = points.back().expectedTime;
	// Add the path between
	path = this->getEntity()->pathfinder->findPathBetweenPoints(points.back().pos, to.arrivingPath.front().getPos(), departTime, getSpeed());
	points.insert(points.end(), path.begin(), path.end());
	departTime = points.back().expectedTime;
	// Add the arriving path
	path = Utils::speedPointVectorToRoutePointVector(to.arrivingPath, departTime, getSpeed());
	points.insert(points.end(), path.begin(), path.end());

	// Now just set the distance between all the points
	for (auto it = points.begin() + 1; it != points.end(); it++) {
		it->distance = (it - 1)->distance + Utils::getVectorDistance((it - 1)->pos, it->pos);
	}
	return points;
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
		VehicleModelInfo mInfo = VehicleModelInfo::getModelInfo(this->model);
		VehicleControllerStop fromStop = this->stops[this->stopIndex - 1];
		VehicleControllerStop toStop = this->stops[this->stopIndex];
		// Go to the stop
		this->points = getPathBetweenStops(fromStop, toStop);
		this->pointIndex = 1;
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
	// The first stop (departing the depot) should have the same time as the route
	if (!stops.empty()) {
		this->stops[0].expectedTime = this->departTime;
		this->stops[0].distance = 0;
	}
}
float VehicleController::getSpeed() {
	return VehicleModelInfo::getModelInfo(this->model).getSpeed();
}
void VehicleController::fromSaveData(SaveData data) {
	this->stopIndex = std::stoull(data.getValue("stopIndex"));
	this->departTime = std::stoull(data.getValue("departTime"));
	// Update the expected time for every stop up to stopIndex
	// Since we need that information to move accordingly
	this->stops[0].expectedTime = departTime;
	this->stops[0].distance = 0;
	for (size_t i = 1; i < this->stopIndex; i++) {
		RoutePoint lastPointInPath = getPathBetweenStops(this->stops[i - 1], this->stops[i]).back();
		this->stops[i].expectedTime = lastPointInPath.expectedTime;
		this->stops[i].distance = lastPointInPath.distance;
	}
	VehicleControllerStop fromStop = this->stops[this->stopIndex - 1];
	VehicleControllerStop toStop = this->stops[this->stopIndex];
	this->points = getPathBetweenStops(fromStop, toStop);
	this->pointIndex = std::stoull(data.getValue("pointIndex"));
	// Load cargo cars
	size_t numCargoCars = std::stoull(data.getValue("numCargoCars"));
	this->cargoCars = std::vector<std::weak_ptr<Entity>>(numCargoCars);
	for (SaveData d : data.getDatas()) {
		if (d.getName() == "CargoCar") {
			size_t index = std::stoull(d.getValue("index"));
			size_t id = std::stoull(d.getValue("id"));
			this->cargoCars[index] = this->getEntity()->getGame()->getEntityById(id);
		}
	}
}
std::optional<SaveData> VehicleController::getSaveData() {
	SaveData data("VehicleController");
	data.addValue("stopIndex", stopIndex);
	data.addValue("pointIndex", pointIndex);
	data.addValue("departTime", departTime);
	data.addValue("numCargoCars", this->cargoCars.size());
	for (auto it = this->cargoCars.begin(); it != this->cargoCars.end(); it++) {
		if (it->lock()) {
			SaveData cargoCarData("CargoCar");
			cargoCarData.addValue("index", it - this->cargoCars.begin());
			cargoCarData.addValue("id", it->lock()->getId());
			data.addData(cargoCarData);
		}
	}
	return { data };
}

std::pair<sf::Vector3f, IsoRotation> VehicleController::getPosAndRotAtDistance(float distance) {
	// First, find the first stop it would have passed
	float initialDistance = 0.0f;
	for (auto it = this->stops.begin(); it != this->stops.end() - 1; it++) {
		auto path = getPathBetweenStops(*it, *(it + 1));
		initialDistance = path.back().distance;
		RoutePoint prevPoint = path.front();
		for (auto p : path) {
			if (p.distance > distance) {
				// Get the vector of going to p from prevPoint
				sf::Vector3f diff = p.pos - prevPoint.pos;
				// Turn it into a unit vector
				sf::Vector3f unit = diff / sqrt(pow(diff.x, 2) + pow(diff.y, 2) + pow(diff.z, 2));
				// Get the rotation
				IsoRotation rot = IsoRotation::fromUnitVector(unit);
				// Calculate how far it would get
				sf::Vector3f pos = prevPoint.pos + unit * (distance - prevPoint.distance);
				return { pos, rot };
			}
			else {
				prevPoint = p;
			}
		}
	}
}
void VehicleController::deleteCars() {
	for (auto c : this->cargoCars) {
		this->getEntity()->getGame()->removeEntity(c);
	}
}

void VehicleController::onArriveAtTile(sf::Vector2i point) {}
void VehicleController::onArriveAtStop(size_t stopIndex) {}
void VehicleController::onArriveAtDest() {}
