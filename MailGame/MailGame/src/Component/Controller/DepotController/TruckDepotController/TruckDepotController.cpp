#include "TruckDepotController.h"
#include "Entity/Entity.h"
#include "Entity/EntityPresets/VehiclePresets/VehiclePresets.h"
#include "Game/Game.h"
#include "Component/Transform/Transform.h"
#include "Component/TransitStop/TransitStop.h"

TruckDepotController::TruckDepotController() : DepotController(TransitType::Car, { VehicleModel::SlowCargoTruck, VehicleModel::FastCargoTruck }) {}
void TruckDepotController::spawnVehicleForRoute(TransitRoute route) {
	gtime_t spawnTime = this->getEntity()->getGame()->getMidnightTime() + Game::UNITS_IN_GAME_HOUR * route.departTime;
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