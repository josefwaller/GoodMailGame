#include "TrainController.h"
#include "Entity/Entity.h"
#include "Component/Ai/Ai.h"
#include "Component/Pathfinder/Pathfinder.h"
#include "Component/Transform/Transform.h"
#include "Game/Game.h"
#include "System/Utils/Utils.h"
#include "GameMap/GameMap.h"

TrainController::TrainController(gtime_t departTime, VehicleModel model, std::vector<std::weak_ptr<Entity>> trainCars) : VehicleController(departTime, model, trainCars), stopIndex(0), state(State::DepartingStation), isBlocked(false), lockedTiles({}) {}

void TrainController::update(float delta) {
	if (this->stopIndex == 0) {
		// Get the stops
		this->stops = this->getEntity()->ai->getStops();
		this->stopIndex = 1;
		// Depart from the first station
		std::weak_ptr<Entity> station = this->stops.front().getEntityTarget();
		this->setPoints(Utils::speedPointVectorToRoutePointVector(TransitStop::getDepartingTransitPath(station.lock(), TransitType::Train), this->departTime, this->model));
	}
	if (this->isBlocked) {
		std::vector<SpeedPoint> arrivingPath = TransitStop::getArrivingTransitPath(this->stops.at(this->stopIndex).getEntityTarget().lock(), TransitType::Train);
		std::vector<SpeedPoint> points = this->getEntity()->pathfinder->findPathBetweenPoints(this->getEntity()->transform->getPosition(), arrivingPath.front().getPos(), this->getEntity()->getGame()->getTime(), this->getSpeed());
		sf::Vector2i nextTile = Utils::toVector2i(points.at(1).getPos());
		// Check if we already have a lock on the tile we need to go to
		// (Can sometimes happen when leaving a station)
		if (std::find(this->lockedTiles.begin(), this->lockedTiles.end(), nextTile) != this->lockedTiles.end()) {
			this->isBlocked = false;
			this->setPoints(Utils::speedPointVectorToRoutePointVector(points, this->getEntity()->getGame()->getTime(), this->model, 0.0f));
		}
		else
			// Try to get a lock on the tile
			if (this->getEntity()->getGameMap()->canGetTileLock(nextTile.x, nextTile.y, TransitType::Train)) {
				this->getEntity()->getGameMap()->getTileLock(nextTile.x, nextTile.y, TransitType::Train);
				this->lockedTiles.push_back(nextTile);
				this->isBlocked = false;
				this->setPoints(Utils::speedPointVectorToRoutePointVector(points, this->getEntity()->getGame()->getTime(), this->model, 0.0f));
			}
	}
	else {
		VehicleController::update(delta);
	}
}

void TrainController::onArriveAtDest(gtime_t arriveTime) {
	sf::Vector3f from;
	sf::Vector3f to;
	switch (this->state) {
	case State::DepartingStation:
		this->state = State::InTransit;
		// Now set points to the path between stations
		from = this->points.back().pos;
		to = TransitStop::getArrivingTransitPath(this->stops.at(this->stopIndex).getEntityTarget().lock(), TransitType::Train).front().getPos();
		this->setPoints(Utils::speedPointVectorToRoutePointVector(
			this->getEntity()->pathfinder->findPathBetweenPoints(from, to, arriveTime, this->getSpeed()),
			this->points.back().expectedTime,
			this->model,
			0.0f));
		break;
	case State::InTransit:
		this->state = State::ArriveAtStation;
		// Arrive at the station
		// Eventually there will be some cool logic here to determine where at the station to arrive at
		this->setPoints(Utils::speedPointVectorToRoutePointVector(TransitStop::getArrivingTransitPath(this->stops.at(this->stopIndex).getEntityTarget().lock(), TransitType::Train), arriveTime, this->model));
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
			this->setPoints(Utils::speedPointVectorToRoutePointVector(TransitStop::getDepartingTransitPath(this->stops.at(this->stopIndex - 1).getEntityTarget().lock(), TransitType::Train), arriveTime, this->model));
		}
		break;
	}
}

void TrainController::onArriveAtPoint(size_t pointIndex, gtime_t arriveTime) {
	this->getEntity()->ai->onArriveAtTile(Utils::toVector2i(this->points.at(pointIndex).pos));
	// Get a lock on the next point
	sf::Vector2i nextTile = Utils::toVector2i(this->points.at(this->pointIndex).pos);
	if (this->getEntity()->getGameMap()->canGetTileLock(nextTile.x, nextTile.y, TransitType::Train)) {
		this->getEntity()->getGameMap()->getTileLock(nextTile.x, nextTile.y, TransitType::Train);
		this->lockedTiles.push_back(nextTile);
	}
	else {
		this->isBlocked = true;
	}
	// Remove any tiles that we don't need locks for anymore
	int numTiles = (int)ceil(1 + this->cargoCars.size() * 0.3f);
	for (int i = this->lockedTiles.size() - 1 - numTiles; i >= 0; i--) {
		this->getEntity()->getGameMap()->releaseTileLock(this->lockedTiles.at(i).x, this->lockedTiles.at(i).y, TransitType::Train);
		this->lockedTiles.erase(this->lockedTiles.begin() + i);
	}
}

void TrainController::onDelete() {
	for (sf::Vector2i tile : this->lockedTiles) {
		this->getEntity()->getGameMap()->releaseTileLock(tile.x, tile.y, TransitType::Train);
	}
	// From VehicleController
	this->deleteCars();
}