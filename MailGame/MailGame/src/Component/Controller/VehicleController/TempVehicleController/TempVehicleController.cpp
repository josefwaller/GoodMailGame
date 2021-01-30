#include "TempVehicleController.h"
#include "Entity/Entity.h"
#include "Component/Ai/Ai.h"
#include "Game/Game.h"
#include "System/Utils/Utils.h"
#include "Component/Pathfinder/Pathfinder.h"

TempVehicleController::TempVehicleController(gtime_t departTime, VehicleModel model, std::vector<std::weak_ptr<Entity>> cargoCars) : VehicleController(departTime, model, cargoCars), stopIndex(0) {}

void TempVehicleController::update(float delta) {
	if (this->stops.empty()) {
		this->stops = this->getEntity()->ai->getStops();
		this->stopIndex = 1;
		this->stops[0].expectedTime = this->departTime;
		this->setPoints(this->getPathBetweenStops(this->stops[0], this->stops[1], this->departTime));
	}
	VehicleController::update(delta);
}

void TempVehicleController::onArriveAtPoint(size_t pointIndex, gtime_t arriveTime) {
	sf::Vector3f pos = this->points[pointIndex].pos;
	this->getEntity()->ai->onArriveAtTile(sf::Vector2i(floor(pos.x), floor(pos.y)));
}
void TempVehicleController::onArriveAtDest(gtime_t arriveTime) {
	this->stopIndex++;
	if (this->stopIndex >= this->stops.size()) {
		this->getEntity()->ai->onArriveAtDest();
	}
	else {
		this->getEntity()->ai->onArriveAtStop(this->stopIndex - 1);
		this->setPoints(this->getPathBetweenStops(this->stops[this->stopIndex - 1], this->stops[this->stopIndex], arriveTime));
	}
}