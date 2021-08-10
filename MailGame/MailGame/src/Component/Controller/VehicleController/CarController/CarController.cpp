#include "CarController.h"
#include "Entity/Entity.h"
#include "Component/Ai/Ai.h"
#include "Component/Transform/Transform.h"
#include "Component/ComponentType/ComponentType.h"
#include "Game/Game.h"
#include "System/Utils/Utils.h"
#include "Component/Pathfinder/Pathfinder.h"

CarController::CarController(gtime_t departTime, VehicleModel model, std::vector<std::weak_ptr<Entity>> cargoCars) : VehicleController(departTime, model, cargoCars), stopIndex(0) {}

void CarController::init() {
	this->stops = this->getEntity()->ai->getStops();
	this->stopIndex = 1;
	this->stops[0].expectedTime = this->departTime;
	this->setPoints(Utils::speedPointVectorToRoutePointVector(
		this->getPathBetweenStops(this->stops[this->stopIndex - 1], this->stops[this->stopIndex]),
		this->departTime,
		this->model,
		0.0f));
}

void CarController::update(float delta) {
	VehicleController::update(delta);
}

void CarController::onArriveAtPoint(size_t pointIndex, gtime_t arriveTime) {
	sf::Vector2i tile = Utils::toVector2i(this->points.at(pointIndex).pos - sf::Vector3f(0.5f, 0.5f, 0));
	this->getEntity()->ai->onArriveAtTile(tile);
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
	// Get the path between
	this->path = Pathfinder::findCarPath(this->getEntity()->getGameMap(), from, to);
	auto pFront = this->path.front();
	if (!std::holds_alternative<sf::Vector2i>(pFront)) {
		throw std::runtime_error("Front of path must always be a point!");
	}
	sf::Vector2i prevPoint = std::get<sf::Vector2i>(this->path.front());
	std::vector<SpeedPoint> points = { sf::Vector3f(prevPoint.x + 0.5f, prevPoint.y + 0.5f, this->getEntity()->getGameMap()->getHeightAt(prevPoint.x + 0.5f, prevPoint.y + 0.5f)) };
	for (auto it = this->path.begin() + 1; it != this->path.end(); it++) {
		if (std::holds_alternative<sf::Vector2i>(*it)) {
			sf::Vector2i thisPoint = std::get<sf::Vector2i>(*it);
			sf::Vector3f nextPoint((prevPoint.x + thisPoint.x) / 2.0f + 0.5f, (prevPoint.y + thisPoint.y) / 2.0f + 0.5f, 0.0f);
			// Set the height
			nextPoint.z = this->getEntity()->getGameMap()->getHeightAt(nextPoint.x, nextPoint.y);
			points.push_back(nextPoint);
			prevPoint = thisPoint;
		}
		else if (std::holds_alternative<Tunnel>(*it)) {
			Tunnel toGoThrough = std::get<Tunnel>(*it);
			auto entrances = toGoThrough.getEntrances();
			std::vector<sf::Vector3f> tunnelPoints;
			sf::Vector2i exitTile;
			if (prevPoint + sf::Vector2i(entrances.first.getDirection().getUnitVector()) == Utils::toVector2i(entrances.first.getPosition())) {
				// Go through the tunnel starting from the first direction
				tunnelPoints = toGoThrough.getPoints();
				exitTile = Utils::toVector2i(entrances.second.getPosition()) + Utils::toVector2i(entrances.second.getDirection().getReverse().getUnitVector());
			}
			else {
				tunnelPoints = toGoThrough.getPoints();
				std::reverse(tunnelPoints.begin(), tunnelPoints.end());
				exitTile = Utils::toVector2i(entrances.first.getPosition()) + Utils::toVector2i(entrances.first.getDirection().getReverse().getUnitVector());
			}
			for (sf::Vector3f p : tunnelPoints) {
				// Here should be the tunnel speed logic
				points.push_back(SpeedPoint(p));
			}
			points.push_back(SpeedPoint(sf::Vector3f(exitTile.x + 0.5f, exitTile.y + 0.5f, this->getEntity()->getGameMap()->getHeightAt(sf::Vector2f(exitTile) + sf::Vector2f(0.5f, 0.5f)))));
			prevPoint = exitTile;
		}
		else {
			throw std::runtime_error("Invalid type of path segment!");
		}
	}
	// Add the last point
	sf::Vector2i lastPoint = std::get<sf::Vector2i>(path.back());
	points.push_back(sf::Vector3f(lastPoint.x + 0.5f, lastPoint.y + 0.5f, this->getEntity()->getGameMap()->getHeightAt(lastPoint.x + 0.5f, lastPoint.y + 0.5f)));
	// Start and end at rest
	points.at(0).setSpeed(0.0f);
	points.at(points.size() - 1).setSpeed(0.0f);
	return points;
}

std::vector<sf::Vector2i> CarController::getDockTiles(std::shared_ptr<Entity> e) {
	// Basically just returns the car docks if we'rek going to a warehouse
	switch (e->tag) {
	case EntityTag::Warehouse: return this->getConnectedDocks(e, EntityTag::CarDock);
	default: return { Utils::toVector2i(e->transform->getPosition()) };
	}
}

std::optional<SaveData> CarController::getSaveData() {
	using namespace SaveKeys;
	SaveData data(componentTypeToStr(ComponentType::Controller));
	data.addVehicleControllerStopVector(STOPS, this->stops);
	data.addSizeT(STOP_INDEX, this->stopIndex);
	data.addData(VehicleController::getSaveData().value());
	return data;
}

void CarController::fromSaveData(SaveData data) {
	using namespace SaveKeys;
	VehicleController::fromSaveData(data.getData(VEHICLE_CONTROLLER));
	this->stops = data.getVehicleControllerStopVector(STOPS, this->getEntity()->getGame());
	this->stopIndex = data.getSizeT(STOP_INDEX);
}