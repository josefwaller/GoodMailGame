#include "TrainController.h"
#include "Entity/Entity.h"
#include "Component/Ai/Ai.h"
#include "Component/Pathfinder/Pathfinder.h"
#include "System/Utils/Utils.h"

TrainController::TrainController(gtime_t departTime, VehicleModel model, std::vector<std::weak_ptr<Entity>> trainCars) : VehicleController(departTime, model, trainCars), stopIndex(0) {}

void TrainController::update(float delta) {
	if (this->stopIndex == 0) {
		this->stops = this->getEntity()->ai->getStops();
		this->stops[0].expectedTime = departTime;
		this->stopIndex = 1;
		this->setPoints(this->getPathBetweenStops(this->stops.front(), this->stops.at(1), this->departTime));
	}
	VehicleController::update(delta);
}

void TrainController::onArriveAtDest(gtime_t arriveTime) {
	this->stopIndex++;
	if (this->stopIndex >= this->stops.size()) {
		this->getEntity()->ai->onArriveAtDest();
	}
	else {
		this->getEntity()->ai->onArriveAtStop(this->stopIndex - 1);
		this->setPoints(this->getPathBetweenStops(this->stops.at(this->stopIndex - 1), this->stops.at(this->stopIndex), arriveTime));
	}
}

void TrainController::onArriveAtPoint(size_t pointIndex, gtime_t arriveTime) {
	this->getEntity()->ai->onArriveAtTile(Utils::toVector2i(this->points.at(pointIndex).pos));
}