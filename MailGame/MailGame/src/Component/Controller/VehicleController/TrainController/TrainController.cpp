#include "TrainController.h"
#include "Entity/Entity.h"
#include "Component/Ai/Ai.h"
#include "Component/Pathfinder/Pathfinder.h"
#include "System/Utils/Utils.h"

TrainController::TrainController(gtime_t departTime, VehicleModel model, std::vector<std::weak_ptr<Entity>> trainCars) : VehicleController(departTime, model, trainCars), stopIndex(0), state(State::DepartingStation) {}

void TrainController::update(float delta) {
	if (this->stopIndex == 0) {
		// Get the stops
		this->stops = this->getEntity()->ai->getStops();
		this->stops[0].expectedTime = departTime;
		this->stopIndex = 1;
		// Depart from the first station
		this->setPoints(Utils::speedPointVectorToRoutePointVector(this->stops.front().departingPath, this->departTime, this->getSpeed()));
	}
	VehicleController::update(delta);
}

void TrainController::onArriveAtDest(gtime_t arriveTime) {
	sf::Vector3f from;
	sf::Vector3f to;
	switch (this->state) {
	case State::DepartingStation:
		this->state = State::InTransit;
		// Now set points to the path between stations
		from = this->points.back().pos;
		to = this->stops.at(this->stopIndex).arrivingPath.front().getPos();
		this->setPoints(this->getEntity()->pathfinder->findPathBetweenPoints(from, to, arriveTime, this->getSpeed()));
		break;
	case State::InTransit:
		this->state = State::ArriveAtStation;
		// Arrive at the station
		// Eventually there will be some cool logic here to determine where at the station to arrive at
		this->setPoints(Utils::speedPointVectorToRoutePointVector(this->stops.at(this->stopIndex).arrivingPath, arriveTime, this->getSpeed()));
		break;
	case State::ArriveAtStation:
		this->state = State::DepartingStation;
		// Has arrived
		this->stopIndex++;
		if (this->stopIndex >= this->stops.size()) {
			this->getEntity()->ai->onArriveAtDest();
		}
		else {
			this->getEntity()->ai->onArriveAtStop(this->stopIndex - 1);
			this->setPoints(Utils::speedPointVectorToRoutePointVector(this->stops.at(this->stopIndex - 1).departingPath, arriveTime, this->getSpeed()));
		}
		break;
	}
}

void TrainController::onArriveAtPoint(size_t pointIndex, gtime_t arriveTime) {
	this->getEntity()->ai->onArriveAtTile(Utils::toVector2i(this->points.at(pointIndex).pos));
}