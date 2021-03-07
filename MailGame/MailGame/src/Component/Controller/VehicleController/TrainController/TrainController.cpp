#include "TrainController.h"
#include "Entity/Entity.h"
#include "Component/Ai/Ai.h"
#include "Component/Pathfinder/Pathfinder.h"
#include "Component/Transform/Transform.h"
#include "Game/Game.h"
#include "System/Utils/Utils.h"
#include "GameMap/GameMap.h"

TrainController::TrainController(gtime_t departTime, VehicleModel model, std::vector<std::weak_ptr<Entity>> trainCars) : VehicleController(departTime, model, trainCars), stopIndex(0), state(State::InTransit), isBlocked(false), lockedTiles({}) {}

void TrainController::update(float delta) {
	if (this->stopIndex == 0) {
		// Get the stops
		this->stops = this->getEntity()->ai->getStops();
		this->stopIndex = 1;
		// Depart from the depot
		std::weak_ptr<Entity> depot = this->stops.front().getEntityTarget();
		// Get a path from the depot path to the first station
		// TODO: It should maybe go over all the points returned from getDockPath(depot) first?
		auto depart = this->getDockPath(depot.lock());
		auto arrive = this->getDockPath(this->stops.at(this->stopIndex).getEntityTarget().lock());
		this->setPoints(Utils::speedPointVectorToRoutePointVector(this->getEntity()->pathfinder->findPathBetweenPoints(depart.front().getPos(), arrive.front().getPos(), this->departTime, getSpeed()), this->departTime, this->model));
	}
	if (this->isBlocked) {
		std::vector<SpeedPoint> arrivingPath = this->getDockPath(this->stops.at(this->stopIndex).getEntityTarget().lock());
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
	case State::ArriveAtStation:
		this->state = State::InTransit;
		this->stopIndex++;
		if (this->stopIndex >= this->stops.size()) {
			this->getEntity()->ai->onArriveAtDest();
		}
		else {
			this->getEntity()->ai->onArriveAtStop(this->stopIndex - 1);
			// Now set points to the path between stations
			from = this->points.back().pos;
			to = this->getDockPath(this->stops.at(this->stopIndex).getEntityTarget().lock()).front().getPos();
			this->setPoints(Utils::speedPointVectorToRoutePointVector(
				this->getEntity()->pathfinder->findPathBetweenPoints(from, to, arriveTime, this->getSpeed()),
				this->points.back().expectedTime,
				this->model,
				this->points.back().speedAtPoint));
		}
		break;
	case State::InTransit:
		this->state = State::ArriveAtStation;
		// Arrive at the station
		// Eventually there will be some cool logic here to determine where at the station to arrive at
		this->setPoints(Utils::speedPointVectorToRoutePointVector(this->getDockPath(this->stops.at(this->stopIndex).getEntityTarget().lock()), arriveTime, this->model, this->points.back().speedAtPoint));
		break;
	}
}

void TrainController::onArriveAtPoint(size_t pointIndex, gtime_t arriveTime) {
	this->getEntity()->ai->onArriveAtTile(Utils::toVector2i(this->points.at(pointIndex).pos));
	// Get a lock on the next point
	if (this->points.size() < this->pointIndex) {
		sf::Vector2i nextTile = Utils::toVector2i(this->points.at(this->pointIndex).pos);
		if (this->getEntity()->getGameMap()->canGetTileLock(nextTile.x, nextTile.y, TransitType::Train)) {
			this->getEntity()->getGameMap()->getTileLock(nextTile.x, nextTile.y, TransitType::Train);
			this->lockedTiles.push_back(nextTile);
		}
		else {
			if (std::find(this->lockedTiles.begin(), this->lockedTiles.end(), nextTile) == this->lockedTiles.end()) {
				this->isBlocked = true;
			}
		}
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

std::vector<SpeedPoint> TrainController::getDockPath(std::shared_ptr<Entity> e) {
	if (e->tag == EntityTag::Warehouse) {
		// Find all the docks attached to the warehouse
		std::vector<sf::Vector2i> availableDocks = getConnectedDocks(e, EntityTag::TrainDock);
		// Now look for a train station near the dock
		sf::Vector2i pos = availableDocks.front();
		for (int x = -1; x < 2; x++) {
			for (int y = -1; y < 2; y++) {
				if (x == 0 || y == 0) {
					auto r = this->getEntity()->getGameMap()->getTileAt(pos.x + x, pos.y + y).railway;
					if (r.has_value() && r.value().isStation) {
						// Find how far back the train station goes
						pos = sf::Vector2i(pos.x + x, pos.y + y);
						std::vector<SpeedPoint> points;// = { SpeedPoint(sf::Vector3f(pos.x + x, pos.y + y, 0.0f), 0.5f) };
						while (r.has_value() && r.value().isStation) {
							points.push_back(SpeedPoint(sf::Vector3f(pos.x, pos.y, 0), 0.5f));
							pos = pos + Utils::toVector2i(r.value().from.getUnitVector());
							r = e->getGameMap()->getTileAt(pos.x, pos.y).railway;
						}
						points[0] = SpeedPoint(points.at(0).getPos(), 0.0f);
						std::reverse(points.begin(), points.end());
						return points;
					}
				}
			}
		}
		throw std::runtime_error("No path mean crash right now");
	}
	return { e->transitStop->getTrainStop().tile };
}