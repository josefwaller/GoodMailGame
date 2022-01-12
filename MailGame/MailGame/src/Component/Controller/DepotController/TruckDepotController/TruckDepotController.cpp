#include "TruckDepotController.h"
#include "Entity/Entity.h"
#include "Entity/EntityPresets/VehiclePresets/VehiclePresets.h"
#include "Game/Game.h"
#include "Component/Transform/Transform.h"
#include "Component/TransitStop/TransitStop.h"
#include "System/Utils/Utils.h"
#include "Component/Pathfinder/Pathfinder.h"

TruckDepotController::TruckDepotController() : DepotController(TransitType::Car, { VehicleModel::SlowCargoTruck, VehicleModel::FastCargoTruck }) {}
void TruckDepotController::spawnVehicleForRoute(TransitRoute route) {
	gtime_t spawnTime = this->getEntity()->getGame()->getMidnightTime() + route.departTime;
	this->getEntity()->getGame()->addEntity(
		VehiclePresets::cargoTruck(
			this->getEntity()->getGame(),
			this->getEntity()->transitStop->getCarStop().tile,
			IsoRotation::NORTH,
			prepareRouteForVehicle(route),
			this->getEntity(),
			spawnTime
		)
	);
}

const char* TruckDepotController::getUiName() {
	return "Truck Depot";
}

void TruckDepotController::resetRouteLength(size_t routeId) {
	sf::Vector2i start = Utils::toVector2i(this->getEntity()->transform->getPosition());
	std::vector<sf::Vector2i> stops;
	for (TransitRouteStop stop : this->routes.at(routeId).stops) {
		if (stop.target.lock()) {
			// For cars, we just go directly ontop of the entity
			stops.push_back(Utils::toVector2i(stop.target.lock()->transform->getPosition()));
		}
	}
	this->routes.at(routeId).length = Pathfinder::getCarRouteLength(this->getEntity()->getGameMap(), start, stops);
}