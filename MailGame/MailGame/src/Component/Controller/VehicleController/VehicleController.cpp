#include "VehicleController.h"

#include "Game/Game.h"
#include "System/IsoRotation/IsoRotation.h"
#include "Component/Transform/Transform.h"
#include "Component/Pathfinder/Pathfinder.h"
#include "Entity/Entity.h"
#include "System/Utils/Utils.h"
#include "System/SaveData/SaveData.h"
#include "Component/Renderer/IsoSpriteRenderer/IsoSpriteRenderer.h"
#include "VehicleModel/VehicleModel.h"
#include "Component/Ai/Ai.h"
#include <SFML/System/Vector3.hpp>
#include <queue>
#include <map>
#include <memory>
#include <functional>
#include <math.h>

VehicleController::VehicleController(gtime_t d, VehicleModel m, std::vector<std::weak_ptr<Entity>> cargoCars)
	: departTime(d), pointIndex(0), model(m), cargoCars(cargoCars) {}

void VehicleController::update(float delta) {
	gtime_t travelTime = this->getEntity()->getGame()->getTime();// -this->departTime;
	if (this->pointIndex >= this->points.size()) {
		if (!this->points.empty())
			this->onArriveAtDest(this->points.back().expectedTime);
		else
			this->onArriveAtDest(this->departTime);
	}
	else {
		// If the truck has gone through all the points
		// Also called if the truck has no points, i.e. no path exists
		if (travelTime >= this->points[this->pointIndex].expectedTime) {
			RoutePoint point = this->points[this->pointIndex];
			// Go to the next point
			this->pointIndex++;
			if (this->pointIndex >= this->points.size()) {
				this->onArriveAtDest(this->points.back().expectedTime);
				return;
			}
			// Add the next tile to traversed points
			this->traversedPoints.push_back(this->points[this->pointIndex]);
			// Callback for children classes
			this->onArriveAtPoint(this->pointIndex - 1, this->points[this->pointIndex - 1].expectedTime);
		}
		else {
			RoutePoint lastPoint = this->points[this->pointIndex - 1];
			RoutePoint nextPoint = this->points[this->pointIndex];
			gtime_t timeSincePoint = travelTime - lastPoint.expectedTime;
			sf::Vector3f pos = lastPoint.pos + (nextPoint.pos - lastPoint.pos) * (float)(timeSincePoint / (float)(nextPoint.expectedTime - lastPoint.expectedTime));
			this->getEntity()->transform->setPosition(pos);
			// Get rotation
			if (lastPoint.pos != nextPoint.pos) {
				sf::Vector2f diff(nextPoint.pos.x - lastPoint.pos.x, nextPoint.pos.y - lastPoint.pos.y);
				IsoRotation rot = IsoRotation::fromUnitVector(diff);
				this->getEntity()->transform->setRotation(rot);
			}
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

std::vector<RoutePoint> VehicleController::getPathBetweenStops(VehicleControllerStop from, VehicleControllerStop to, gtime_t d) {
	std::vector<RoutePoint> points;
	float speed = VehicleModelInfo::getModelInfo(this->model).getSpeed();
	// First, it always starts at the first point in the departing path when departing from
	points.push_back(RoutePoint(from.departingPath.front().getPos(), from.expectedTime, from.distance));
	gtime_t departTime = from.expectedTime;
	// Add the departing path, the vehicle departs after waiting
	auto path = Utils::speedPointVectorToRoutePointVector(from.departingPath, departTime + from.waitTime, speed);
	points.insert(points.end(), path.begin(), path.end());
	departTime = points.back().expectedTime;
	// Add the path between
	path = this->getEntity()->pathfinder->findPathBetweenPoints(points.back().pos, to.arrivingPath.front().getPos(), departTime, speed);
	points.insert(points.end(), path.begin(), path.end());
	departTime = points.back().expectedTime;
	// Add the arriving path
	path = Utils::speedPointVectorToRoutePointVector(to.arrivingPath, departTime, speed);
	points.insert(points.end(), path.begin(), path.end());

	// Now just set the distance between all the points
	for (auto it = points.begin() + 1; it != points.end(); it++) {
		it->distance = (it - 1)->distance + Utils::getVectorDistance((it - 1)->pos, it->pos);
	}
	return points;
}

float VehicleController::getSpeed() {
	return VehicleModelInfo::getModelInfo(this->model).getSpeed();
}
void VehicleController::fromSaveData(SaveData data) {
	this->departTime = std::stoull(data.getValue("departTime"));
	// Update the expected time for every stop up to stopIndex
	// Since we need that information to move accordingly
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
	for (auto it = this->traversedPoints.begin() + 1; it != this->traversedPoints.end(); it++) {
		if (it->distance > distance) {
			auto prev = it - 1;
			sf::Vector3f diff = it->pos - prev->pos;
			// Turn it into a unit vector
			sf::Vector3f unit = diff / sqrt(pow(diff.x, 2) + pow(diff.y, 2) + pow(diff.z, 2));
			// Get the rotation
			IsoRotation rot = IsoRotation::fromUnitVector(unit);
			// Calculate how far it would get
			sf::Vector3f pos = prev->pos + unit * (distance - prev->distance);
			return { pos, rot };
		}
	}
}
void VehicleController::onDelete() {
	this->deleteCars();
}
void VehicleController::deleteCars() {
	for (auto c : this->cargoCars) {
		this->getEntity()->getGame()->removeEntity(c);
	}
}

void VehicleController::setCargoCars(std::vector<std::weak_ptr<Entity>> cc) {
	//this->deleteCars();
	this->cargoCars = cc;
}
void VehicleController::onArriveAtPoint(size_t pointIndex, gtime_t arriveTime) {}
void VehicleController::onArriveAtDest(gtime_t arriveTime) {}
void VehicleController::setPoints(std::vector<RoutePoint> points) {
	// Set the points' distance, using traversed points
	RoutePoint prevPoint = this->traversedPoints.empty() ? points.front() : this->traversedPoints.back();
	for (auto it = points.begin(); it != points.end(); it++) {
		it->distance = prevPoint.distance + Utils::getVectorDistance(prevPoint.pos, it->pos);
		prevPoint = *it;
	}
	this->points = points;
	this->pointIndex = 1;
	// Since we skip this point
	// This will crash if points isn't at least 2 long
	if (!this->points.empty()) {
		this->traversedPoints.push_back(this->points.front());
		if (this->points.size() > 1) {
			this->traversedPoints.push_back(this->points.at(1));
		}
	}
}