#include "PlaneController.h"
#include "Entity/Entity.h"
#include "Component/Pathfinder/Pathfinder.h"
#include "System/Utils/Utils.h"
#include "Component/Ai/Ai.h"
#include "Game/Game.h"
#include "Component/Transform/Transform.h"
#include "Component/ComponentType/ComponentType.h"
#include <queue>

// This will eventually be gone
const float TAXI_SPEED = 1.0f;

PlaneController::PlaneController(gtime_t departTime, VehicleModel model) : VehicleController(departTime, model), stops({}), stopIndex(0), state(State::InDepot), runway(sf::Vector2i(), sf::Vector2i()) {}

void PlaneController::init() {
	this->stops = this->getEntity()->ai->getStops();
}
void PlaneController::update(float delta) {
	// This will eventually be changed hopefully, as it's a bit messy atm
	if (this->state == State::InDepot) {
		if (this->stopIndex == this->stops.size() - 1) {
			// Done
			this->getEntity()->getGame()->removeEntity(this->getEntity());
		}
		else {
			// Try to taxi to the destination first
			sf::Vector3f fromPoint = this->traversedPoints.empty() ? this->getEntity()->transform->getPosition() : this->traversedPoints.back().pos;
			auto path = this->getTaxiPathToEntity(Utils::toVector2i(fromPoint), this->stops.at(this->stopIndex + 1).getEntityTarget().lock());
			if (!path.empty()) {
				this->stopIndex++;
				this->state = State::TaxiingToDest;
				gtime_t time = this->traversedPoints.empty() ? this->getEntity()->getGame()->getTime() : this->traversedPoints.back().expectedTime;
				this->setPoints(Utils::speedPointVectorToRoutePointVector(this->setupTaxiPath(path), time, this->model, 0.0f));
			}
			else {
				// Currently at stops[stopIndex]
				// So try and find a runway to take off from
				std::vector<Runway> runways = getAllRunwaysForEntity(this->stops.at(this->stopIndex).getEntityTarget().lock());
				// Right now we just care if a runway exists
				for (Runway r : runways) {
					if (this->tryGetLockOnRunway(r)) {
						this->runway = r;
						this->stopIndex++;
						this->state = State::TaxiingToRunway;
						// TODO: Right now we just round position
						// Shouldn't probably do this
						sf::Vector3f pos = this->getEntity()->transform->getPosition();
						sf::Vector2i p(round(pos.x), round(pos.y));
						std::vector<sf::Vector3f> path = this->getTaxiPathToRunway(p, this->runway);
						// Set the points
						this->setPoints(Utils::speedPointVectorToRoutePointVector(setupTaxiPath(path), this->getEntity()->getGame()->getTime(), this->model, 0.0f));
						break;
					}
				}
			}
		}
	}
	else if (this->state == State::WaitingForArrivalLock) {
		std::vector<Runway> runways = this->getAllRunwaysForEntity(this->stops.at(this->stopIndex).getEntityTarget().lock());
		for (Runway r : runways) {
			if (this->tryGetLockOnRunway(r)) {
				this->runway = r;
				this->state = State::ArrivingAtRunway;
				// We actually need the last 2 points since we may have not arrived at this point yet
				std::vector<SpeedPoint> points = { SpeedPoint(this->traversedPoints.back().pos) };
				auto runwayPoints = getRunwayArrivePoints(this->runway);
				points.insert(points.end(), runwayPoints.begin(), runwayPoints.end());
				auto rPoints = (Utils::speedPointVectorToRoutePointVector(
					points,
					this->traversedPoints.back().expectedTime,
					this->model,
					this->traversedPoints.back().speedAtPoint
				));
				rPoints.insert(rPoints.begin(), this->traversedPoints.at(this->traversedPoints.size() - 2));
				this->setPoints(rPoints);
				break;
			}
		}
		VehicleController::update(delta);
	}
	else {
		VehicleController::update(delta);
	}
}

void PlaneController::onArriveAtDest(gtime_t arriveTime) {
	VehicleModelInfo modelInfo = VehicleModelInfo::getModelInfo(this->model);
	switch (this->state) {
	case State::InDepot: {
		break;
	}
	case State::Departing: {
		this->state = State::InTransit;
		this->releaseLocks();
		std::shared_ptr<Entity> e = this->stops.at(this->stopIndex).getEntityTarget().lock();
		this->setPoints(
			Utils::speedPointVectorToRoutePointVector(
				this->getEntity()->pathfinder->findPathBetweenPoints(this->points.back().pos, e->transform->getPosition() + sf::Vector3f(0, 0, 3.0f), arriveTime, this->getSpeed()),
				this->points.back().expectedTime, this->model, this->points.back().speedAtPoint));
		break;
	}
	case State::WaitingForArrivalLock:
	case State::InTransit: {
		// Handled by update
		this->state = State::WaitingForArrivalLock;
		this->setPoints(Utils::speedPointVectorToRoutePointVector(this->getLoopPath(this->points.back()), this->points.back().expectedTime, this->model, this->points.back().speedAtPoint));
		break;
	}
	case State::ArrivingAtRunway:
		this->state = State::TaxiingToDest;
		this->releaseLocks();
		this->setPoints(Utils::speedPointVectorToRoutePointVector(this->setupTaxiPath(this->getTaxiPathToEntity(this->runway.start, this->stops.at(this->stopIndex).getEntityTarget().lock())), this->points.back().expectedTime, this->model, this->points.back().speedAtPoint));
		break;
	case State::TaxiingToRunway:
		this->state = State::Departing;
		this->setPoints(Utils::speedPointVectorToRoutePointVector(getRunwayDepartPoints(this->runway), this->points.back().expectedTime, this->model));
		break;
	case State::TaxiingToDest:
		this->state = State::InDepot;
		this->setPoints({ this->points.back(), RoutePoint(this->points.back().pos, this->points.back().expectedTime + modelInfo.getLoadTime() + modelInfo.getUnloadTime(), 0.0f, 0.0f) });
		if (this->stopIndex < this->stops.size()) {
			this->getEntity()->ai->onArriveAtStop(this->stopIndex);
		}
		else {
			this->getEntity()->ai->onArriveAtDest();
		}
		break;
	}
}

std::vector<SpeedPoint> PlaneController::getLoopPath(RoutePoint p) {
	const float X_OFF = 3.0f;
	const float Y_OFF = 3.0f;
	return {
		SpeedPoint(p.pos),
		SpeedPoint(p.pos + sf::Vector3f(X_OFF, 0, 0)),
		SpeedPoint(p.pos + sf::Vector3f(X_OFF, Y_OFF, 0)),
		SpeedPoint(p.pos + sf::Vector3f(0, Y_OFF, 0)),
		SpeedPoint(p.pos)
	};
}

PlaneController::Runway::Runway(sf::Vector2i start, sf::Vector2i end) : start(start), end(end) {}

std::vector<PlaneController::Runway> PlaneController::getAllRunwaysForEntity(std::shared_ptr<Entity> e) {
	std::vector<sf::Vector2i> startingPoints;
	if (e->tag == EntityTag::Warehouse) {
		// Find a connected dock
		startingPoints = getConnectedDocks(e, EntityTag::AirplaneDock);
	}
	else if (e->tag == EntityTag::Airport) {
		startingPoints.push_back(Utils::toVector2i(e->transform->getPosition()));
	}
	// Simply do a search
	std::vector<sf::Vector2i> possible = startingPoints;
	std::vector<sf::Vector2i> visited;
	std::vector<sf::Vector2i> runwayStarts;
	GameMap* gMap = this->getEntity()->getGameMap();
	// this algorithm currently assumes all taxi roads point in all directions
	while (!possible.empty()) {
		sf::Vector2i current = possible.back();
		possible.pop_back();
		for (int x = -1; x < 2; x++) {
			for (int y = -1; y < 2; y++) {
				if (x == 0 || y == 0) {
					sf::Vector2i newPos(current.x + x, current.y + y);
					if (std::find(visited.begin(), visited.end(), newPos) == visited.end()) {
						visited.push_back(newPos);
						if (gMap->getTileAt(newPos.x, newPos.y).airplaneRoad.has_value()) {
							AirplaneRoad r = gMap->getTileAt(newPos.x, newPos.y).airplaneRoad.value();
							if (r.isRunway) {
								if (std::find(runwayStarts.begin(), runwayStarts.end(), newPos) == runwayStarts.end()) {
									runwayStarts.push_back(newPos);
								}
							}
							else {
								possible.push_back(newPos);
							}
						}
					}
				}
			}
		}
	}
	if (runwayStarts.empty()) {
		// damn
		throw std::runtime_error(":(");
	}
	// Now get the runway from the possible starts
	std::vector<Runway> toReturn;
	for (sf::Vector2i pos : runwayStarts) {
		sf::Vector2i start = pos;
		AirplaneRoad r = gMap->getTileAt(pos.x, pos.y).airplaneRoad.value();
		sf::Vector2i unit;
		if (r.hasNorth && gMap->getTileAt(pos.x, pos.y - 1).airplaneRoad.value_or(AirplaneRoad()).isRunway) {
			unit = sf::Vector2i(0, -1);
		}
		else if (r.hasSouth && gMap->getTileAt(pos.x, pos.y + 1).airplaneRoad.value_or(AirplaneRoad()).isRunway) {
			unit = sf::Vector2i(0, 1);
		}
		else if (r.hasEast && gMap->getTileAt(pos.x - 1, pos.y).airplaneRoad.value_or(AirplaneRoad()).isRunway) {
			unit = sf::Vector2i(-1, 0);
		}
		else {
			unit = sf::Vector2i(1, 0);
		}
		while (gMap->getTileAt(pos.x + unit.x, pos.y + unit.y).airplaneRoad.value_or(AirplaneRoad()).isRunway) {
			pos += unit;
		}
		toReturn.push_back(Runway(start, pos));
	}
	return toReturn;
}
std::vector<sf::Vector3f> PlaneController::getTaxiPath(sf::Vector2i from, std::vector<sf::Vector2i> to) {
	std::vector<sf::Vector2i> potential = { from };
	std::vector<sf::Vector2i> visited;
	auto previous = std::map<sf::Vector2i, sf::Vector2i, std::function<bool(sf::Vector2i one, sf::Vector2i two)>>{
		[](sf::Vector2i one, sf::Vector2i two) {
			return 1000 * one.x + one.y > 1000 * two.x + two.y;
		}
	};
	while (!potential.empty()) {
		sf::Vector2i current = potential.back();
		potential.pop_back();
		if (std::find(visited.begin(), visited.end(), current) == visited.end()) {
			visited.push_back(current);
			Tile t = this->getEntity()->getGame()->getGameMap()->getTileAt(current);
			if (!t.airplaneRoad.has_value()) {
				continue;
			}
			std::vector<sf::Vector3i> connected = t.airplaneRoad.value().getConnectedTiles(Utils::toVector3i(current));
			for (sf::Vector3i c : connected) {
				sf::Vector2i newPos = Utils::toVector2i(c);
				if (std::find(to.begin(), to.end(), newPos) != to.end()) {
					std::vector<sf::Vector3f> toReturn = { Utils::toVector3f(newPos), Utils::toVector3f(current) };
					while (current != from) {
						current = previous.at(current);
						toReturn.push_back(Utils::toVector3f(current));
					}
					std::reverse(toReturn.begin(), toReturn.end());
					return toReturn;
				}
				Tile otherTile = this->getEntity()->getGameMap()->getTileAt(newPos);
				if (otherTile.airplaneRoad.has_value()) {
					potential.push_back(newPos);
					previous.insert({ newPos, current });
				}
			}
		}
	}
	return {};
}
std::vector<sf::Vector3f> PlaneController::getTaxiPathToRunway(sf::Vector2i pos, Runway to) {
	return getTaxiPath(pos, { to.start });
}
std::vector<sf::Vector3f> PlaneController::getTaxiPathToEntity(sf::Vector2i pos, std::shared_ptr<Entity> e) {
	std::vector<sf::Vector2i> destPoints;
	if (e->tag == EntityTag::Warehouse) {
		destPoints = getConnectedDocks(e, EntityTag::AirplaneDock);
	}
	else if (e->tag == EntityTag::Airport) {
		destPoints.push_back(Utils::toVector2i(e->transform->getPosition()));
	}
	return getTaxiPath(pos, destPoints);
}
std::vector<SpeedPoint> PlaneController::setupTaxiPath(std::vector<sf::Vector3f> path) {
	std::vector<SpeedPoint> speedPath = { SpeedPoint(path.front(), 0.0f) };
	for (auto it = path.begin() + 1; it != path.end() - 1; it++) {
		speedPath.push_back(SpeedPoint(*it, TAXI_SPEED));
	}
	speedPath.push_back(SpeedPoint(path.back(), 0.0f));
	return speedPath;
}

std::vector<SpeedPoint> PlaneController::getRunwayArrivePoints(Runway r) {
	sf::Vector3f from = Utils::toVector3f(r.end);
	sf::Vector3f to = Utils::toVector3f(r.start);
	return {
		SpeedPoint(from + Utils::getUnitVector(from - to) * 3.0f + sf::Vector3f(0, 0, 3.0f)),
		SpeedPoint(from),
		SpeedPoint(to, 0.0f)
	};
}

bool PlaneController::tryGetLockOnRunway(Runway r) {
	sf::Vector2i unit = Utils::getUnitVector(r.end - r.start);
	sf::Vector2i pos = r.start;
	while (true) {
		if (!this->getEntity()->getGameMap()->canGetTileLock(pos.x, pos.y, TransitType::Airplane)) {
			return false;
		}
		if (pos == r.end) {
			break;
		}
		pos += unit;
	}
	// Now actually get the lock
	for (pos = r.start; pos != r.end + unit; pos += unit) {
		this->getEntity()->getGameMap()->getTileLock(pos.x, pos.y, TransitType::Airplane);
		this->lockedTiles.push_back(pos);
	}
	return true;
}

void PlaneController::releaseLocks() {
	for (sf::Vector2i p : this->lockedTiles) {
		this->getEntity()->getGameMap()->releaseTileLock(p.x, p.y, TransitType::Airplane);
	}
	this->lockedTiles.clear();
}

std::vector<SpeedPoint> PlaneController::getRunwayDepartPoints(Runway r) {
	auto s = getRunwayArrivePoints(r);
	std::reverse(s.begin(), s.end());
	return s;
}

std::optional<SaveData> PlaneController::getSaveData() {
	using namespace SaveKeys;
	SaveData data(componentTypeToStr(ComponentType::Controller));
	data.addVehicleControllerStopVector(STOPS, this->stops);
	data.addSizeT(STOP_INDEX, this->stopIndex);
	data.addSizeT(STATE, (size_t)this->state);
	data.addVector2iVector(LOCKED_TILES, this->lockedTiles);
	SaveData runwayData(RUNWAY);
	runwayData.addVector2i(START, this->runway.start);
	runwayData.addVector2i(END, this->runway.end);
	data.addData(runwayData);
	data.addData(VehicleController::getSaveData().value());
	return data;
}

void PlaneController::fromSaveData(SaveData data) {
	using namespace SaveKeys;
	VehicleController::fromSaveData(data.getData(VEHICLE_CONTROLLER));
	this->stops = data.getVehicleControllerStopVector(STOPS, this->getEntity()->getGame());
	this->stopIndex = data.getSizeT(STOP_INDEX);
	this->state = (State)data.getSizeT(STATE);
	this->lockedTiles = data.getVector2iVector(LOCKED_TILES);
	SaveData runwayData = data.getData(RUNWAY);
	this->runway = {
		runwayData.getVector2i(START),
		runwayData.getVector2i(END)
	};
}