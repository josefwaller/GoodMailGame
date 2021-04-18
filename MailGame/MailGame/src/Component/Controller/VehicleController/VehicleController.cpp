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
#include "Component/MailContainer/MailContainer.h"
#include <SFML/System/Vector3.hpp>
#include <queue>
#include <map>
#include <memory>
#include <functional>
#include <math.h>
#include <imgui.h>

bool VehicleControllerStop::hasEntityTarget() {
	return std::holds_alternative<std::weak_ptr<Entity>>(this->target);
}
bool VehicleControllerStop::hasTileTarget() {
	return std::holds_alternative<sf::Vector2i>(this->target);
}
std::weak_ptr<Entity> VehicleControllerStop::getEntityTarget() {
	return std::get<std::weak_ptr<Entity>>(this->target);
}
sf::Vector2i VehicleControllerStop::getTileTarget() {
	return std::get<sf::Vector2i>(this->target);
}

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
			// Callback for children classes
			this->onArriveAtPoint(this->pointIndex - 1, this->points[this->pointIndex - 1].expectedTime);
			if (this->pointIndex >= this->points.size()) {
				this->onArriveAtDest(this->points.back().expectedTime);
				return;
			}
			// Add the next tile to traversed points
			this->traversedPoints.push_back(this->points[this->pointIndex]);
		}
		else {
			RoutePoint lastPoint = this->points[this->pointIndex - 1];
			RoutePoint nextPoint = this->points[this->pointIndex];
			sf::Vector3f unit = Utils::getUnitVector(nextPoint.pos - lastPoint.pos);
			float timeSincePoint = (travelTime - lastPoint.expectedTime) / (float)Game::UNITS_IN_GAME_HOUR;
			// Calculate position based on d = v * t + 1 / 2 * a * t ^ 2
			// Note that this may note reach the next point by nextPoint.expectedTime
			// That is ok, we just hope at this point that the numbers add up
			sf::Vector3f pos = lastPoint.pos + unit * (lastPoint.speedAtPoint * timeSincePoint + 0.5f * lastPoint.accelerationAtPoint * powf(timeSincePoint, 2));
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

void VehicleController::renderUi() {
	if (ImGui::Begin(this->getEntity()->getName().c_str())) {
		size_t numMail = this->getEntity()->mailContainer->getNumLetters();
		char buf[200];
		sprintf_s(buf, "%zu letters", numMail);
		ImGui::Text(buf);
		ImGui::End();
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
float VehicleController::getSpeed() {
	return VehicleModelInfo::getModelInfo(this->model).getSpeed();
}
void VehicleController::fromSaveData(SaveData data) {
	using namespace SaveKeys;
	this->departTime = data.getGTimeT(DEPART_TIME);
	// Update the expected time for every stop up to stopIndex
	// Since we need that information to move accordingly
	this->pointIndex = data.getSizeT(POINT_INDEX);
	// Load cargo cars
	size_t numCargoCars = data.getSizeT(NUM_CARGO_CARS);
	this->cargoCars = std::vector<std::weak_ptr<Entity>>(numCargoCars);
	for (SaveData d : data.getDatas()) {
		if (d.getName() == CARGO_CAR) {
			this->cargoCars[d.getSizeT(INDEX)] = this->getEntity()->getGame()->getEntityById(d.getSizeT(ID));
		}
	}
	this->points = data.getRoutePointVector(POINTS);
	this->traversedPoints = data.getRoutePointVector(TRAVERSED_POINTS);
}
std::optional<SaveData> VehicleController::getSaveData() {
	using namespace SaveKeys;
	SaveData data(VEHICLE_CONTROLLER);
	data.addSizeT(POINT_INDEX, pointIndex);
	data.addGTimeT(DEPART_TIME, departTime);
	data.addSizeT(NUM_CARGO_CARS, this->cargoCars.size());
	for (auto it = this->cargoCars.begin(); it != this->cargoCars.end(); it++) {
		if (it->lock()) {
			SaveData cargoCarData(CARGO_CAR);
			cargoCarData.addSizeT(INDEX, it - this->cargoCars.begin());
			cargoCarData.addSizeT(ID, it->lock()->getId());
			data.addData(cargoCarData);
		}
	}
	// Add points
	data.addRoutePointVector(POINTS, this->points);
	data.addRoutePointVector(TRAVERSED_POINTS, this->traversedPoints);
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
			sf::Vector3f unit = diff / (float)(sqrt(pow(diff.x, 2) + pow(diff.y, 2) + pow(diff.z, 2)));
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
		if (c.lock())
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

std::vector<sf::Vector2i> VehicleController::getConnectedDocks(std::shared_ptr<Entity> e, EntityTag dockTag) {
	// Find all the docks attached to the warehouse
	std::vector<sf::Vector2i> availableDocks;
	sf::Vector2i targetPos = Utils::toVector2i(e->transform->getPosition());
	std::vector<sf::Vector2i> warehousePos = {
		sf::Vector2i(targetPos.x, targetPos.y),
		sf::Vector2i(targetPos.x, targetPos.y + 1),
		sf::Vector2i(targetPos.x + 1, targetPos.y),
		sf::Vector2i(targetPos.x + 1, targetPos.y + 1)
	};
	for (sf::Vector2i pos : warehousePos) {
		for (int x = -1; x < 2; x++) {
			for (int y = -1; y < 2; y++) {
				if (x == 0 || y == 0) {
					std::weak_ptr<Entity> building = this->getEntity()->getGameMap()->getTileAt((size_t)(pos.x + x), (size_t)(pos.y + y)).building;
					if (building.lock() && building.lock()->tag == dockTag) {
						availableDocks.push_back(sf::Vector2i(pos.x + x, pos.y + y));
					}
				}
			}
		}
	}
	return availableDocks;
}