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
	auto depart = this->getDockTiles(depot.lock()).front();
	auto arrive = this->getDockTiles(this->stops.at(this->stopIndex).getEntityTarget().lock());
	this->getEntity()->transform->setRotation(depot.lock()->transform->getRotation());
	// Set path
	this->tryGetUnblocked(depart, arrive, depot.lock()->transform->getRotation(), 0.0f);
}

void TrainController::tryGetUnblocked(sf::Vector2i from, std::vector<sf::Vector2i> to, IsoRotation rot, float startingSpeed) {
	auto allPaths = RailsPathfinder::findRailwayPath(from, to, rot, this->getEntity()->getGameMap());
	for (auto path : allPaths) {
		// Check if we can get the segment
		if (this->tryGetLockForSegment(path.front())) {
			this->state = State::InTransit;
			this->pathIndex = 0;
			this->path = path;
			// Set points
			this->setPoints(Utils::speedPointVectorToRoutePointVector(this->getPointsForSegment(this->path.at(this->pathIndex), this->pathIndex == this->path.size() - 1 ? 0.0f : std::optional<int>()), this->getEntity()->getGame()->getTime(), this->model, startingSpeed));
			return;
		}
	}
	this->state = State::Blocked;
}

void TrainController::update(float delta) {
	switch (this->state) {
	case State::Blocked: {
		IsoRotation blockedRotation = this->lockedRailways.empty() ? this->getEntity()->transform->getRotation() : this->lockedRailways.back().second.getTo();
		this->tryGetUnblocked(Utils::toVector2i(this->getEntity()->transform->getPosition()), this->getDockTiles(this->stops.at(this->stopIndex).getEntityTarget().lock()), blockedRotation, this->traversedPoints.empty() ? 0.0f : this->traversedPoints.back().speedAtPoint);
	}
	}
	VehicleController::update(delta);
}

bool TrainController::tryGetLockForSegment(RailsPathfinder::Segment segment) {
	// Check if we can get a lock for the entire segment
	using Type = RailsPathfinder::Segment::Type;
	switch (segment.getType()) {
	case Type::Path: {
		auto rails = segment.getPath();
		for (auto kv : rails) {
			if (std::find_if(this->lockedRailways.begin(), this->lockedRailways.end(), [&kv](std::pair<sf::Vector2i, Railway> p) { return p.first == kv.first && p.second.getTo() == kv.second.getTo(); }) == this->lockedRailways.end()) {
				if (!this->getEntity()->getGameMap()->getTileAt(kv.first).canGetRailwayLock(kv.second)) {
					return false;
				}
			}
		}
		// Get the lock
		for (auto kv : rails) {
			this->getEntity()->getGameMap()->getRailwayLock(kv.first, kv.second);
			this->lockedRailways.push_back(kv);
		}
	}
	case Type::Tunnel:
		return true;
	}
	return true;
}

void TrainController::onArriveAtDest(gtime_t arriveTime) {
	switch (this->state) {
	case State::InTransit:
		// If we just arrived at a station
		this->pathIndex++;
		if (this->pathIndex >= this->path.size()) {
			this->stopIndex++;
			if (this->stopIndex >= this->stops.size()) {
				this->getEntity()->ai->onArriveAtDest();
			}
			else {
				this->getEntity()->ai->onArriveAtStop(this->stopIndex - 1);
				// Now set points to the path between stations
				sf::Vector2i from = Utils::toVector2i(this->points.back().pos);
				std::vector<sf::Vector2i> to = this->getDockTiles(this->stops.at(this->stopIndex).getEntityTarget().lock());
				this->tryGetUnblocked(from, to, this->lockedRailways.back().second.getTo(), this->traversedPoints.back().speedAtPoint);
			}
		}
		else {
			if (this->tryGetLockForSegment(this->path.at(this->pathIndex))) {
				this->setPoints(Utils::speedPointVectorToRoutePointVector(this->getPointsForSegment(this->path.at(this->pathIndex), this->pathIndex == this->path.size() - 1 ? 0.0f : std::optional<int>()), arriveTime, this->model, this->traversedPoints.back().speedAtPoint));
			}
			else {
				// Check for any other possibilities
				this->tryGetUnblocked(this->lockedRailways.back().first, this->getDockTiles(this->stops.at(this->stopIndex).getEntityTarget().lock()), this->lockedRailways.back().second.getTo(), this->traversedPoints.back().speedAtPoint);
			}
		}
	}
}
void TrainController::onArriveAtPoint(gtime_t arriveTime, size_t pointIndex) {
}

std::vector<SpeedPoint> TrainController::getPointsForSegment(RailsPathfinder::Segment s, std::optional<int> endingSpeed) {
	using Type = RailsPathfinder::Segment::Type;
	switch (s.getType()) {
	case Type::Path: {
		auto path = s.getPath();
		if (path.empty()) {
			return {};
		}
		// Start from the from direction of the first tile
		sf::Vector2f frontPoint = sf::Vector2f(path.front().first) + sf::Vector2f(0.5f, 0.5f) + 0.5f * path.front().second.getFrom().getUnitVector();
		std::vector<SpeedPoint> points = { SpeedPoint(sf::Vector3f(frontPoint.x, frontPoint.y, this->getEntity()->getGameMap()->getHeightAt(frontPoint))) };
		for (auto p : path) {
			sf::Vector2f point = sf::Vector2f(p.first) + sf::Vector2f(0.5f, 0.5f) + 0.5f * p.second.getTo().getUnitVector();
			points.push_back(sf::Vector3f(point.x, point.y, this->getEntity()->getGameMap()->getHeightAt(point)));
		}
		if (endingSpeed.has_value()) {
			points.back().setSpeed(endingSpeed.value());
		}
		return points;
	}
	case Type::Tunnel: {
		std::vector<SpeedPoint> toReturn;
		auto points = s.getTunnel().getPoints();
		if (s.getTunnelReversed()) {
			std::reverse(points.begin(), points.end());
		}
		for (auto p : points) {
			toReturn.push_back(SpeedPoint(p));
		}
		return toReturn;
	}
	}
}

void TrainController::onDelete() {
	// From VehicleController
	this->deleteCars();
	while (!this->lockedRailways.empty()) {
		auto p = this->lockedRailways.back();
		this->lockedRailways.pop_back();
		this->getEntity()->getGameMap()->releaseRailwayLock(p.first, p.second);
	}
}

std::vector<sf::Vector2i> TrainController::getDockTiles(std::shared_ptr<Entity> e) {
	if (e->tag == EntityTag::Warehouse) {
		// Find all the docks attached to the warehouse
		std::vector<sf::Vector2i> availableDocks = getConnectedDocks(e, EntityTag::TrainDock);
		// Now look for a train station near the dock
		std::vector<sf::Vector2i> startingPoints;
		for (sf::Vector2i dock : availableDocks) {
			// Get all the points the docks are facing
			startingPoints.push_back(dock + sf::Vector2i(this->getEntity()->getGameMap()->getTileAt(dock).building.lock()->transform->getRotation().getUnitVector()));
		}
		// Now we simply gather all station tiles that are connected to the starting points
		std::vector<sf::Vector2i> stationTiles;
		for (sf::Vector2i s : startingPoints) {
			auto r = this->getEntity()->getGameMap()->getTileAt(s).getRailways();
			if (!r.empty() && r.front().isStation) {
				stationTiles.push_back(s);
			}
		}
		for (size_t i = 0; i < stationTiles.size(); i++) {
			for (int x = -1; x < 2; x++) {
				for (int y = -1; y < 2; y++) {
					sf::Vector2i tile = stationTiles.at(i) + sf::Vector2i(x, y);
					// Check we are not going back and forth
					if (std::find(stationTiles.begin(), stationTiles.end(), tile) == stationTiles.end()) {
						auto r = this->getEntity()->getGameMap()->getTileAt(tile).getRailways();
						if (!r.empty() && r.front().isStation) {
							stationTiles.push_back(tile);
						}
					}
				}
			}
		}
		// Now we just filter out for the ones at the end
		std::vector<sf::Vector2i> toReturn;
		for (sf::Vector2i t : stationTiles) {
			if (std::find(stationTiles.begin(), stationTiles.end(), t + sf::Vector2i(this->getEntity()->getGameMap()->getTileAt(t).getRailways().front().getTo().getUnitVector())) == stationTiles.end()) {
				toReturn.push_back(t);
			}
		}
		return toReturn;
	}
	else if (e->tag == EntityTag::TrainStation) {
		sf::Vector2i tile = Utils::toVector2i(e->transform->getPosition());
		if (this->getEntity()->getGameMap()->getTileAt(tile).getRailways().empty()) {
			throw std::runtime_error("TrainStation has no railways!");
		}
		return { { tile } };
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