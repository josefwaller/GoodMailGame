#include "TrainController.h"
#include "Entity/Entity.h"
#include "Component/Ai/Ai.h"
#include "Component/Pathfinder/Pathfinder.h"
#include "Component/Transform/Transform.h"
#include "Component/ComponentType/ComponentType.h"
#include "Component/Pathfinder/RailsPathfinder/RailsPathfinder.h"
#include "Game/Game.h"
#include "System/Utils/Utils.h"
#include "GameMap/GameMap.h"

TrainController::TrainController(gtime_t departTime, VehicleModel model, std::vector<std::weak_ptr<Entity>> trainCars) : VehicleController(departTime, model, trainCars), stopIndex(0), state(State::InTransit) {}

void TrainController::init() {
	// Get the stops
	this->stops = this->getEntity()->ai->getStops();
	this->stopIndex = 1;
	// Depart from the depot
	std::weak_ptr<Entity> depot = this->stops.front().getEntityTarget();
	// Get a path from the depot path to the first station
	// TODO: It should maybe go over all the points returned from getDockPath(depot) first?
	auto depart = this->getDockPath(depot.lock());
	auto arrive = this->getDockPath(this->stops.at(this->stopIndex).getEntityTarget().lock());
	this->resetPath(depart.front().first, arrive.front().first, this->getEntity()->transform->getRotation(), this->departTime);
}

void TrainController::update(float delta) {
	VehicleController::update(delta);
}

void TrainController::onArriveAtDest(gtime_t arriveTime) {
	sf::Vector3f from;
	sf::Vector2i to;
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
			to = this->getDockPath(this->stops.at(this->stopIndex).getEntityTarget().lock()).front().first;
			this->resetPath(
				Utils::toVector2i(from),
				to,
				this->path.back().second.getTo(),
				arriveTime);
		}
		break;
	case State::InTransit:
		this->state = State::ArriveAtStation;
		// Arrive at the station
		// Eventually there will be some cool logic here to determine where at the station to arrive at
		this->path = this->getDockPath(this->stops.at(this->stopIndex).getEntityTarget().lock());
		// Stop at the train station
		auto spPath = RailsPathfinder::railwayPathToSpeedPointPath(this->path);
		spPath.back().setSpeed(0.0f);
		this->setPoints(Utils::speedPointVectorToRoutePointVector(spPath, arriveTime, this->model, this->points.back().speedAtPoint));
		break;
	}
}

void TrainController::onArriveAtPoint(size_t pointIndex, gtime_t arriveTime) {
	this->getEntity()->ai->onArriveAtTile(Utils::toVector2i(this->points.at(pointIndex).pos));
	// Get a lock on the next point
	if (this->pointIndex < this->points.size() - 1) {
		GameMap* gMap = this->getEntity()->getGameMap();
		auto p = this->path[pointIndex];
		if (gMap->canGetRailwayLock(p.first, p.second)) {
			gMap->getRailwayLock(p.first, p.second);
		}
	}
}

void TrainController::onDelete() {
	// From VehicleController
	this->deleteCars();
}

std::vector<std::pair<sf::Vector2i, Railway>> TrainController::getDockPath(std::shared_ptr<Entity> e) {
	if (e->tag == EntityTag::Warehouse) {
		// Find all the docks attached to the warehouse
		std::vector<sf::Vector2i> availableDocks = getConnectedDocks(e, EntityTag::TrainDock);
		// Now look for a train station near the dock
		sf::Vector2i pos = availableDocks.front();
		for (int x = -1; x < 2; x++) {
			for (int y = -1; y < 2; y++) {
				if (x == 0 || y == 0) {
					Tile t = this->getEntity()->getGameMap()->getTileAt(pos.x + x, pos.y + y);
					if (t.getRailways().size() == 1 && t.getRailways().front().isStation) {
						// Find how far back the train station goes
						pos = sf::Vector2i(pos.x + x, pos.y + y);
						IsoRotation currentRot;
						for (IsoRotation rot : IsoRotation::CARDINAL_DIRECTIONS) {
							if (!t.getOutgoingRailDirections(rot).empty()) {
								currentRot = rot;
								break;
							}
						}
						std::vector<std::pair<sf::Vector2i, Railway>> path;
						while (t.getRailways().size() == 1 && t.getRailways().front().isStation) {
							path.push_back({ pos, t.getRailways().front() });
							// If it is a station, it must be straight
							pos = pos + Utils::toVector2i(currentRot.getUnitVector());
							t = e->getGameMap()->getTileAt(pos.x, pos.y);
						}
						std::reverse(path.begin(), path.end());
						return path;
					}
				}
			}
		}
		throw std::runtime_error("No path mean crash right now");
	}
	else if (e->tag == EntityTag::TrainStation) {
		sf::Vector2i tile = Utils::toVector2i(e->transitStop->getTrainStop().tile);
		if (this->getEntity()->getGameMap()->getTileAt(tile).getRailways().empty()) {
			throw std::runtime_error("TrainStation has no railways!");
		}
		return { { tile, this->getEntity()->getGameMap()->getTileAt(tile).getRailways().front() } };
	}
}

std::optional<SaveData> TrainController::getSaveData() {
	using namespace SaveKeys;
	SaveData data(componentTypeToStr(ComponentType::Controller));
	data.addVehicleControllerStopVector(STOPS, this->stops);
	data.addSizeT(STOP_INDEX, this->stopIndex);
	data.addSizeT(STATE, (size_t)this->state);
	data.addData(VehicleController::getSaveData().value());
	return data;
}

void TrainController::fromSaveData(SaveData data) {
	using namespace SaveKeys;
	VehicleController::fromSaveData(data.getData(VEHICLE_CONTROLLER));
	this->stops = data.getVehicleControllerStopVector(STOPS, this->getEntity()->getGame());
	this->stopIndex = data.getSizeT(STOP_INDEX);
	this->state = (State)data.getSizeT(STATE);
}

void TrainController::resetPath(sf::Vector2i fromTile, sf::Vector2i toTile, IsoRotation startingRotation, gtime_t time) {
	this->path = RailsPathfinder::findRailwayPath(fromTile, toTile, startingRotation, this->getEntity()->getGameMap(), time);
	// Start at 0 speed
	std::vector<SpeedPoint> points = { SpeedPoint(Utils::toVector3f(fromTile) + sf::Vector3f(0.5f, 0.5f, 0), 0.0f) };
	auto pathPoints = RailsPathfinder::railwayPathToSpeedPointPath(this->path);
	points.insert(points.end(), pathPoints.begin(), pathPoints.end());
	// TODO: Handle path being empty (no path)
	this->setPoints(Utils::speedPointVectorToRoutePointVector(points, time, this->model));
}