#include "PlaneController.h"
#include "Entity/Entity.h"
#include "Component/Pathfinder/Pathfinder.h"
#include "System/Utils/Utils.h"
#include "Component/Ai/Ai.h"

PlaneController::PlaneController(gtime_t departTime, VehicleModel model) : VehicleController(departTime, model), stops({}), stopIndex(0), state(State::InDepot) {}

void PlaneController::onArriveAtDest(gtime_t arriveTime) {
	VehicleModelInfo modelInfo = VehicleModelInfo::getModelInfo(this->model);
	switch (this->state) {
	case State::InDepot:
		if (this->stopIndex == 0) {
			this->stops = this->getEntity()->ai->getStops();
		}
		this->stopIndex++;
		this->state = State::Departing;
		this->setPoints(Utils::speedPointVectorToRoutePointVector(TransitStop::getDepartingTransitPath(this->stops.at(this->stopIndex - 1).getEntityTarget().lock(), TransitType::Airplane), arriveTime, this->getSpeed()));
		break;
	case State::Departing:
		this->state = State::InTransit;
		this->setPoints(this->getEntity()->pathfinder->findPathBetweenPoints(this->points.back().pos, TransitStop::getArrivingTransitPath(this->stops.at(this->stopIndex).getEntityTarget().lock(), TransitType::Airplane).front().getPos(), arriveTime, this->getSpeed()));
		break;
	case State::InTransit:
		this->state = State::ArrivingAtRunway;
		this->setPoints(Utils::speedPointVectorToRoutePointVector(TransitStop::getArrivingTransitPath(this->stops.at(this->stopIndex).getEntityTarget().lock(), TransitType::Airplane), arriveTime, this->getSpeed()));
		break;
	case State::ArrivingAtRunway:
		this->state = State::InDepot;
		this->setPoints({ this->points.back(), RoutePoint(this->points.back().pos, this->points.back().expectedTime + modelInfo.getLoadTime() + modelInfo.getUnloadTime()) });
		break;
	}
}