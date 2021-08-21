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
	// Set path
	this->path = RailsPathfinder::findRailwayPath(depart.front().first, arrive.front().first, depart.front().second.getTo(), this->getEntity()->getGameMap()).front();
	this->pathIndex = 0;
	// Set points
	this->setPoints(Utils::speedPointVectorToRoutePointVector(this->getPointsForSegment(this->path.at(this->pathIndex)), this->departTime, this->model));
}

void TrainController::update(float delta) {
	VehicleController::update(delta);
}

void TrainController::onArriveAtDest(gtime_t arriveTime) {
	sf::Vector3f from;
	sf::Vector2i to;
	switch (this->state) {
	case State::ArrivingAtStation:
		this->stopIndex++;
		if (this->stopIndex >= this->stops.size()) {
			this->getEntity()->ai->onArriveAtDest();
		}
		else {
			this->getEntity()->ai->onArriveAtStop(this->stopIndex - 1);
			this->state = State::InTransit;
			// Now set points to the path between stations
			from = this->points.back().pos;
			to = this->getDockPath(this->stops.at(this->stopIndex).getEntityTarget().lock()).front().first;
			auto p = RailsPathfinder::findRailwayPath(Utils::toVector2i(from), to, this->getEntity()->transform->getRotation(), this->getEntity()->getGameMap());
			if (p.empty()) {
				throw std::runtime_error("Not implemented yet");
			}
			this->path = p.front();
			this->pathIndex = 0;
			this->setPoints(Utils::speedPointVectorToRoutePointVector(this->getPointsForSegment(this->path.at(this->pathIndex)), arriveTime, this->model));
		}
		break;
	case State::InTransit:
		// If we just arrived at a station
		this->pathIndex++;
		if (this->pathIndex >= this->path.size()) {
			this->state = State::ArrivingAtStation;
			// Set points for station
			// Usually, we would go over all the possibilities and find one we can get a lock on, but we're not right now
			auto path = this->getDockPath(this->stops.at(this->stopIndex).getEntityTarget().lock());
			std::vector<SpeedPoint> s = this->getPointsForSegment(RailsPathfinder::Segment(path));
			s.back().setSpeed(0.0f);
			this->setPoints(Utils::speedPointVectorToRoutePointVector(s, arriveTime, this->model, this->points.back().speedAtPoint));
		}
		else {
			this->setPoints(Utils::speedPointVectorToRoutePointVector(this->getPointsForSegment(this->path.at(this->pathIndex)), arriveTime, this->model, this->points.back().speedAtPoint));
		}
	}
}

std::vector<SpeedPoint> TrainController::getPointsForSegment(RailsPathfinder::Segment s) {
	using Type = RailsPathfinder::Segment::Type;
	switch (s.getType()) {
	case Type::Path:
		auto path = s.getPath();
		// Start from the from direction of the first tile
		sf::Vector2f frontPoint = sf::Vector2f(path.front().first) + sf::Vector2f(0.5f, 0.5f) + 0.5f * path.front().second.getFrom().getUnitVector();
		std::vector<SpeedPoint> points = { SpeedPoint(sf::Vector3f(frontPoint.x, frontPoint.y, this->getEntity()->getGameMap()->getHeightAt(frontPoint))) };
		for (auto p : path) {
			sf::Vector2f point = sf::Vector2f(p.first) + sf::Vector2f(0.5f, 0.5f) + 0.5f * p.second.getTo().getUnitVector();
			points.push_back(sf::Vector3f(point.x, point.y, this->getEntity()->getGameMap()->getHeightAt(point)));
		}
		return points;
	}
}

void TrainController::onDelete() {
	// From VehicleController
	this->deleteCars();
	while (!this->lockedRailways.empty()) {
		auto p = this->lockedRailways.front();
		this->lockedRailways.pop();
		this->getEntity()->getGameMap()->releaseRailwayLock(p.first, p.second);
	}
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

float TrainController::getTrainLength() {
	return 2.0f + 0.3f * this->cargoCars.size();
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

sf::Vector2i TrainController::getDest() {
	return this->dest;
}