#include "PlaneController.h"
#include "Entity/Entity.h"
#include "Component/Pathfinder/Pathfinder.h"
#include "System/Utils/Utils.h"
#include "Component/Ai/Ai.h"
#include "Game/Game.h"
#include "Component/Transform/Transform.h"
#include <queue>

// This will eventually be gone
const float TAXI_SPEED = 1.0f;

PlaneController::PlaneController(gtime_t departTime, VehicleModel model) : VehicleController(departTime, model), stops({}), stopIndex(0), state(State::InDepot), runway(sf::Vector2i(), sf::Vector2i()) {}

void PlaneController::update(float delta) {
	// This will eventually be changed hopefully, as it's a bit messy atm
	if (this->state == State::InDepot) {
		if (this->stopIndex == 0) {
			this->stops = this->getEntity()->ai->getStops();
		}
		if (this->stopIndex == this->stops.size() - 1) {
			// Done
			this->getEntity()->getGame()->removeEntity(this->getEntity());
		}
		else {
			// Currently at stops[stopIndex]
			// So try and find a runway to take off from
			std::vector<Runway> runways = getAllRunwaysForEntity(this->stops.at(this->stopIndex).getEntityTarget().lock());
			// Right now we just care if a runway exists
			if (!runways.empty()) {
				this->runway = runways.front();
				this->stopIndex++;
				this->state = State::TaxiingToRunway;
				// TODO: Right now we just round position
				// Shouldn't probably do this
				sf::Vector3f pos = this->getEntity()->transform->getPosition();
				sf::Vector2i p(round(pos.x), round(pos.y));
				std::vector<sf::Vector3f> path = this->getTaxiPathToRunway(p, this->runway);
				// Set the points
				this->setPoints(Utils::speedPointVectorToRoutePointVector(setupTaxiPath(path), this->getEntity()->getGame()->getTime(), this->model, 0.0f));
			}
		}
	}
	else {
		VehicleController::update(delta);
	}
}

void PlaneController::onArriveAtDest(gtime_t arriveTime) {
	VehicleModelInfo modelInfo = VehicleModelInfo::getModelInfo(this->model);
	switch (this->state) {
	case State::InDepot:
		// Handled by update
		break;
	case State::Departing: {
		this->state = State::InTransit;
		std::shared_ptr<Entity> e = this->stops.at(this->stopIndex).getEntityTarget().lock();
		this->setPoints(
			Utils::speedPointVectorToRoutePointVector(
				this->getEntity()->pathfinder->findPathBetweenPoints(this->points.back().pos, e->transform->getPosition() + sf::Vector3f(0, 0, 3.0f), arriveTime, this->getSpeed()),
				this->points.back().expectedTime, this->model, this->points.back().speedAtPoint));
		break;
	}
	case State::InTransit:
	case State::WaitingForLock: {
		// Will eventually be handled by update
		this->state = State::ArrivingAtRunway;
		// Get the attached runway
		std::vector<Runway> runways = this->getAllRunwaysForEntity(this->stops.at(this->stopIndex).getEntityTarget().lock());
		this->runway = runways.front();
		std::vector<SpeedPoint> points = { SpeedPoint(this->points.back().pos) };
		auto runwayPoints = getRunwayArrivePoints(this->runway);
		points.insert(points.end(), runwayPoints.begin(), runwayPoints.end());
		this->setPoints(Utils::speedPointVectorToRoutePointVector(points, this->points.back().expectedTime, this->model, this->points.back().speedAtPoint));
		break;
	}
	case State::ArrivingAtRunway:
		this->state = State::TaxiingToDest;
		this->setPoints(Utils::speedPointVectorToRoutePointVector(this->setupTaxiPath(this->getTaxiPathToEntity(this->runway.start, this->stops.at(this->stopIndex).getEntityTarget().lock())), this->points.back().expectedTime, this->model, this->points.back().speedAtPoint));
		break;
	case State::TaxiingToRunway:
		this->state = State::Departing;
		this->setPoints(Utils::speedPointVectorToRoutePointVector(getRunwayDepartPoints(this->runway), this->points.back().expectedTime, this->model));
		break;
	case State::TaxiingToDest:
		this->state = State::InDepot;
		this->setPoints({ this->points.back(), RoutePoint(this->points.back().pos, this->points.back().expectedTime + modelInfo.getLoadTime() + modelInfo.getUnloadTime(), 0.0f, 0.0f) });
		break;
	}
}

std::vector<RoutePoint> PlaneController::getLoopPath(RoutePoint p) {
	return {
		p,
		RoutePoint(p.pos + sf::Vector3f(3.0f, 0.0f, 0.0f), p.expectedTime + 3.0f * Game::UNITS_IN_GAME_HOUR / this->getSpeed(), this->getSpeed(), 0.0f),
		RoutePoint(p.pos + sf::Vector3f(3.0f, 2.0f, 0.0f), p.expectedTime + 5 * Game::UNITS_IN_GAME_HOUR / this->getSpeed(), this->getSpeed(), 0.0f),
		RoutePoint(p.pos + sf::Vector3f(0.0f, 2.0f, 0.0f), p.expectedTime + 8 * Game::UNITS_IN_GAME_HOUR / this->getSpeed(), this->getSpeed(), 0.0f),
		RoutePoint(p.pos, p.expectedTime + 10 * Game::UNITS_IN_GAME_HOUR / this->getSpeed(), this->getSpeed(), 0.0f)
	};
}

PlaneController::Runway::Runway(sf::Vector2i start, sf::Vector2i end) : start(start), end(end) {}

std::vector<PlaneController::Runway> PlaneController::getAllRunwaysForEntity(std::shared_ptr<Entity> e) {
	if (e->tag == EntityTag::Warehouse) {
		// Find a connected dock
		std::vector<sf::Vector2i> availableDocks = getConnectedDocks(e, EntityTag::AirplaneDock);
		// Simply do a search
		std::vector<sf::Vector2i> possible = availableDocks;
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
	else {
		return {
			Runway(Utils::toVector2i(e->transform->getPosition()) + sf::Vector2i(-1, 1), Utils::toVector2i(e->transform->getPosition()) + sf::Vector2i(1, 1))
		};
	}
}
std::vector<sf::Vector3f> PlaneController::getTaxiPath(sf::Vector2i from, sf::Vector2i to) {
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
			for (int x = -1; x < 2; x++) {
				for (int y = -1; y < 2; y++) {
					if (x == 0 || y == 0) {
						sf::Vector2i newPos(current.x + x, current.y + y);
						if (newPos == to) {
							std::vector<sf::Vector3f> toReturn = { Utils::toVector3f(newPos), Utils::toVector3f(current) };
							while (current != from) {
								current = previous.at(current);
								toReturn.push_back(Utils::toVector3f(current));
							}
							std::reverse(toReturn.begin(), toReturn.end());
							return toReturn;
						}
						if (this->getEntity()->getGameMap()->getTileAt(newPos.x, newPos.y).airplaneRoad.has_value()) {
							potential.push_back(newPos);
							previous.insert({ newPos, current });
						}
					}
				}
			}
		}
	}
	return { Utils::toVector3f(from), Utils::toVector3f(from + to) / 2.0f, Utils::toVector3f(to) };
}
std::vector<sf::Vector3f> PlaneController::getTaxiPathToRunway(sf::Vector2i pos, Runway to) {
	return getTaxiPath(pos, to.start);
}
std::vector<sf::Vector3f> PlaneController::getTaxiPathToEntity(sf::Vector2i pos, std::shared_ptr<Entity> e) {
	if (e->tag == EntityTag::Warehouse) {
		auto docks = getConnectedDocks(e, EntityTag::AirplaneDock);
		std::vector<sf::Vector2i> potential = docks;
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
				for (int x = -1; x < 2; x++) {
					for (int y = -1; y < 2; y++) {
						if (x == 0 || y == 0) {
							sf::Vector2i newPos(current.x + x, current.y + y);
							if (newPos == pos) {
								std::vector<sf::Vector3f> toReturn = { Utils::toVector3f(newPos), Utils::toVector3f(current) };
								while (previous.find(current) != previous.end()) {
									current = previous.at(current);
									toReturn.push_back(Utils::toVector3f(current));
								}
								return toReturn;
							}
							if (this->getEntity()->getGameMap()->getTileAt(newPos.x, newPos.y).airplaneRoad.has_value()) {
								// Add to potential
								potential.push_back(newPos);
								previous.insert({ newPos, current });
							}
						}
					}
				}
			}
		}
		return {};
	}
	return { Utils::toVector3f(pos), (Utils::toVector3f(pos) + e->transform->getPosition()) / 2.0f, e->transform->getPosition() };
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

std::vector<SpeedPoint> PlaneController::getRunwayDepartPoints(Runway r) {
	auto s = getRunwayArrivePoints(r);
	std::reverse(s.begin(), s.end());
	return s;
}