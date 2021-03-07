#include "CarController.h"
#include "Entity/Entity.h"
#include "Component/Ai/Ai.h"
#include "Component/Transform/Transform.h"
#include "Game/Game.h"
#include "System/Utils/Utils.h"
#include "Component/Pathfinder/Pathfinder.h"

CarController::CarController(gtime_t departTime, VehicleModel model, std::vector<std::weak_ptr<Entity>> cargoCars) : VehicleController(departTime, model, cargoCars), stopIndex(0) {}

void CarController::update(float delta) {
	if (this->stops.empty()) {
		this->stops = this->getEntity()->ai->getStops();
		this->stopIndex = 1;
		this->stops[0].expectedTime = this->departTime;
		this->setPoints(Utils::speedPointVectorToRoutePointVector(
			this->getPathBetweenStops(this->stops[0], this->stops[1]),
			this->departTime,
			this->model));
	}
	VehicleController::update(delta);
}

void CarController::onArriveAtPoint(size_t pointIndex, gtime_t arriveTime) {
	sf::Vector3f pos = this->points[pointIndex].pos;
	this->getEntity()->ai->onArriveAtTile(sf::Vector2i(floor(pos.x), floor(pos.y)));
}
void CarController::onArriveAtDest(gtime_t arriveTime) {
	this->stopIndex++;
	if (this->stopIndex >= this->stops.size()) {
		this->getEntity()->ai->onArriveAtDest();
	}
	else {
		this->getEntity()->ai->onArriveAtStop(this->stopIndex - 1);
		this->setPoints(Utils::speedPointVectorToRoutePointVector(
			this->getPathBetweenStops(this->stops[this->stopIndex - 1], this->stops[this->stopIndex]),
			this->points.back().expectedTime,
			this->model,
			this->points.back().speedAtPoint));
	}
}

std::vector<SpeedPoint> CarController::getPathBetweenStops(VehicleControllerStop fromStop, VehicleControllerStop toStop) {
	float speed = VehicleModelInfo::getModelInfo(this->model).getSpeed();
	std::vector<sf::Vector2i> fromTiles = fromStop.hasEntityTarget() ? this->getDockTiles(fromStop.getEntityTarget().lock()) : std::vector<sf::Vector2i>({ fromStop.getTileTarget() });
	std::vector<sf::Vector2i> toTiles = toStop.hasEntityTarget() ? this->getDockTiles(toStop.getEntityTarget().lock()) : std::vector<sf::Vector2i>({ toStop.getTileTarget() });
	// TODO: Add checking if a valid dock even exists for both targets
	// For now, we just go to the first
	sf::Vector2i from = fromTiles.front();
	sf::Vector2i to = toTiles.front();
	// Add the path between
	return this->getEntity()->pathfinder->findPathBetweenPoints(Utils::toVector3f(from), Utils::toVector3f(to), this->departTime, speed);
}

std::vector<sf::Vector2i> CarController::getDockTiles(std::shared_ptr<Entity> e) {
	// Basically just returns the car docks if we'rek going to a warehouse
	switch (e->tag) {
	case EntityTag::Warehouse: return this->getConnectedDocks(e, EntityTag::CarDock);
	default: return { Utils::toVector2i(e->transform->getPosition()) };
	}
}