#include "PlaneController.h"
#include "Entity/Entity.h"
#include "Component/Pathfinder/Pathfinder.h"
#include "System/Utils/Utils.h"
#include "Component/Ai/Ai.h"
#include "Game/Game.h"
#include <queue>

PlaneController::PlaneController(gtime_t departTime, VehicleModel model) : VehicleController(departTime, model), stops({}), stopIndex(0), state(State::InDepot) {}

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
			this->stopIndex++;
			this->state = State::Departing;
			this->setPoints(Utils::speedPointVectorToRoutePointVector(this->getDepartingPath(this->stops.at(this->stopIndex - 1).getEntityTarget().lock()), this->getEntity()->getGame()->getTime(), this->model));
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
	case State::Departing:
		this->state = State::InTransit;
		this->setPoints(
			Utils::speedPointVectorToRoutePointVector(
				this->getEntity()->pathfinder->findPathBetweenPoints(this->points.back().pos, this->getArrivingPath(this->stops.at(this->stopIndex).getEntityTarget().lock()).front().getPos(), arriveTime, this->getSpeed()),
				this->points.back().expectedTime, this->model, this->points.back().speedAtPoint));
		break;
	case State::InTransit:
	case State::WaitingForLock:
		this->state = State::ArrivingAtRunway;
		this->setPoints(Utils::speedPointVectorToRoutePointVector(this->getArrivingPath(this->stops.at(this->stopIndex).getEntityTarget().lock()), arriveTime, this->model, this->points.back().speedAtPoint));
		break;
	case State::ArrivingAtRunway:
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

std::vector<SpeedPoint> PlaneController::getArrivingPath(std::shared_ptr<Entity> e) {
	if (e->tag == EntityTag::Warehouse) {
		auto path = getDepartingPath(e);
		std::reverse(path.begin(), path.end());
		return path;
	}
	else {
		return TransitStop::getArrivingTransitPath(e, TransitType::Airplane);
	}
}

std::vector<SpeedPoint> PlaneController::getDepartingPath(std::shared_ptr<Entity> e) {
	if (e->tag == EntityTag::Warehouse) {
		// Find a connected dock
		std::vector<sf::Vector2i> availableDocks = getConnectedDocks(e, EntityTag::AirplaneDock);
		// For now, just work with the first dock
		sf::Vector2i dock = availableDocks.front();
		// Pathfind to the first runway we can find
		std::vector<sf::Vector2i> visited;
		auto previous = std::map<sf::Vector2i, sf::Vector2i, std::function<bool(sf::Vector2i one, sf::Vector2i two)>>{
			[](sf::Vector2i one, sf::Vector2i two) {
				return 1000 * one.x + one.y > 1000 * two.x + two.y;
			}
		};
		sf::Vector2i current = dock;
		std::queue<sf::Vector2i> next;
		while (true) {
			visited.push_back(current);
			for (int x = -1; x < 2; x++) {
				for (int y = -1; y < 2; y++) {
					if (x != 0 && y != 0) continue;
					sf::Vector2i pos(current.x + x, current.y + y);
					if (std::find(visited.begin(), visited.end(), pos) == visited.end()) {
						GameMap* gMap = this->getEntity()->getGameMap();
						if (gMap->getTileAt(pos.x, pos.y).airplaneRoad.has_value()) {
							next.push(pos);
							AirplaneRoad r = gMap->getTileAt(pos.x, pos.y).airplaneRoad.value();
							// For right now, we don't have to check if the roads line up
							previous.insert({ pos, current });
							if (r.isRunway) {
								sf::Vector2i runwayPos = pos;
								// Build the path up to the runway
								std::vector<SpeedPoint> toReturn = { SpeedPoint(sf::Vector3f(pos.x, pos.y, 0.0f), 0.0f) };
								while (previous.find(pos) != previous.end()) {
									pos = previous.at(pos);
									toReturn.push_back(SpeedPoint(sf::Vector3f(pos.x, pos.y, 0.0f), 1.0f));
								}
								std::reverse(toReturn.begin(), toReturn.end());
								// Done! Now we find where the runway goes
								sf::Vector2i unit;
								if (r.hasNorth && gMap->getTileAt(pos.x, pos.y - 1).airplaneRoad.value_or(AirplaneRoad()).isRunway) {
									unit = sf::Vector2i(0, -1);
								}
								else if (r.hasSouth && gMap->getTileAt(pos.x, pos.y - 1).airplaneRoad.value_or(AirplaneRoad()).isRunway) {
									unit = sf::Vector2i(0, 1);
								}
								else if (r.hasEast && gMap->getTileAt(pos.x - 1, pos.y).airplaneRoad.value_or(AirplaneRoad()).isRunway) {
									unit = sf::Vector2i(-1, 0);
								}
								else {
									unit = sf::Vector2i(1, 0);
								}
								runwayPos += unit;
								while (gMap->getTileAt(runwayPos.x, runwayPos.y).airplaneRoad.value_or(AirplaneRoad()).isRunway) {
									toReturn.push_back(SpeedPoint(sf::Vector3f(runwayPos.x, runwayPos.y, 0.0f)));
									runwayPos += unit;
								}
								// Now add a point above and away from the runway
								sf::Vector3f p(unit.x, unit.y, 1);
								toReturn.push_back(SpeedPoint(toReturn.back().getPos() + p * 5.0f));
								return toReturn;
							}
						}
					}
				}
			}
			current = next.front();
			next.pop();
		}
	}
	else {
		return TransitStop::getDepartingTransitPath(e, TransitType::Airplane);
	}
}