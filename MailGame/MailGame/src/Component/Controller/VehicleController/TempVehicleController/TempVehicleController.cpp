#include "TempVehicleController.h"
#include "Entity/Entity.h"
#include "Component/Ai/Ai.h"
#include "Game/Game.h"
#include "System/Utils/Utils.h"
#include "Component/Pathfinder/Pathfinder.h"

TempVehicleController::TempVehicleController(gtime_t departTime, VehicleModel model, std::vector<std::weak_ptr<Entity>> cargoCars) : VehicleController(departTime, model, cargoCars), stopIndex(0) {}

void TempVehicleController::update(float delta) {
	if (this->stops.empty()) {
		this->stops = this->getEntity()->ai->getStops();
		this->stopIndex = 1;
		this->stops[0].expectedTime = this->departTime;
		this->setPoints(this->getPathBetweenStops(this->stops[0], this->stops[1], this->departTime));
	}
	VehicleController::update(delta);
}

std::vector<RoutePoint> TempVehicleController::getPathBetweenStops(VehicleControllerStop from, VehicleControllerStop to, gtime_t d) {
	std::vector<RoutePoint> points;
	float speed = VehicleModelInfo::getModelInfo(this->model).getSpeed();
	// First, it always starts at the first point in the departing path when departing from
	points.push_back(RoutePoint(from.departingPath.front().getPos(), from.expectedTime, from.distance));
	gtime_t departTime = from.expectedTime;
	// Add the departing path, the vehicle departs after waiting
	auto path = Utils::speedPointVectorToRoutePointVector(from.departingPath, departTime + from.waitTime, speed);
	points.insert(points.end(), path.begin(), path.end());
	departTime = points.back().expectedTime;
	// Add the path between
	path = this->getEntity()->pathfinder->findPathBetweenPoints(points.back().pos, to.arrivingPath.front().getPos(), departTime, speed);
	points.insert(points.end(), path.begin(), path.end());
	departTime = points.back().expectedTime;
	// Add the arriving path
	path = Utils::speedPointVectorToRoutePointVector(to.arrivingPath, departTime, speed);
	points.insert(points.end(), path.begin(), path.end());

	// Now just set the distance between all the points
	for (auto it = points.begin() + 1; it != points.end(); it++) {
		it->distance = (it - 1)->distance + Utils::getVectorDistance((it - 1)->pos, it->pos);
	}
	return points;
}

void TempVehicleController::onArriveAtPoint(size_t pointIndex, gtime_t arriveTime) {
	sf::Vector3f pos = this->points[pointIndex].pos;
	this->getEntity()->ai->onArriveAtTile(sf::Vector2i(floor(pos.x), floor(pos.y)));
}
void TempVehicleController::onArriveAtDest(gtime_t arriveTime) {
	this->stopIndex++;
	if (this->stopIndex >= this->stops.size()) {
		this->getEntity()->ai->onArriveAtDest();
	}
	else {
		this->getEntity()->ai->onArriveAtStop(this->stopIndex - 1);
		this->setPoints(this->getPathBetweenStops(this->stops[this->stopIndex - 1], this->stops[this->stopIndex], arriveTime));
	}
}