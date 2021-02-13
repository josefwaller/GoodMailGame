#include "PlaneController.h"
#include "Entity/Entity.h"
#include "Component/Pathfinder/Pathfinder.h"
#include "System/Utils/Utils.h"
#include "Component/Ai/Ai.h"
#include "Game/Game.h"

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
		else
			if (this->stops.at(this->stopIndex).getEntityTarget().lock()->transitStop->getRunwayLock()) {
				this->stopIndex++;
				this->state = State::Departing;
				this->setPoints(Utils::speedPointVectorToRoutePointVector(TransitStop::getDepartingTransitPath(this->stops.at(this->stopIndex - 1).getEntityTarget().lock(), TransitType::Airplane), this->getEntity()->getGame()->getTime(), this->getSpeed()));
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
		this->stops.at(this->stopIndex - 1).getEntityTarget().lock()->transitStop->releaseRunwayLock();
		this->state = State::InTransit;
		this->setPoints(this->getEntity()->pathfinder->findPathBetweenPoints(this->points.back().pos, TransitStop::getArrivingTransitPath(this->stops.at(this->stopIndex).getEntityTarget().lock(), TransitType::Airplane).front().getPos(), arriveTime, this->getSpeed()));
		break;
	case State::InTransit:
	case State::WaitingForLock:
		if (this->stops.at(this->stopIndex).getEntityTarget().lock()->transitStop->getRunwayLock()) {
			this->state = State::ArrivingAtRunway;
			this->setPoints(Utils::speedPointVectorToRoutePointVector(TransitStop::getArrivingTransitPath(this->stops.at(this->stopIndex).getEntityTarget().lock(), TransitType::Airplane), arriveTime, this->getSpeed()));
		}
		else {
			this->state = State::WaitingForLock;
			this->setPoints(this->getLoopPath(this->points.back()));
		}
		break;
	case State::ArrivingAtRunway:
		this->state = State::InDepot;
		this->setPoints({ this->points.back(), RoutePoint(this->points.back().pos, this->points.back().expectedTime + modelInfo.getLoadTime() + modelInfo.getUnloadTime()) });
		this->stops.at(this->stopIndex).getEntityTarget().lock()->transitStop->releaseRunwayLock();
		break;
	}
}

std::vector<RoutePoint> PlaneController::getLoopPath(RoutePoint p) {
	return {
		p,
		RoutePoint(p.pos + sf::Vector3f(3.0f, 0.0f, 0.0f), p.expectedTime + 3.0f * Game::UNITS_IN_GAME_HOUR / this->getSpeed()),
		RoutePoint(p.pos + sf::Vector3f(3.0f, 2.0f, 0.0f), p.expectedTime + 5 * Game::UNITS_IN_GAME_HOUR / this->getSpeed()),
		RoutePoint(p.pos + sf::Vector3f(0.0f, 2.0f, 0.0f), p.expectedTime + 8 * Game::UNITS_IN_GAME_HOUR / this->getSpeed()),
		RoutePoint(p.pos, p.expectedTime + 10 * Game::UNITS_IN_GAME_HOUR / this->getSpeed())
	};
}