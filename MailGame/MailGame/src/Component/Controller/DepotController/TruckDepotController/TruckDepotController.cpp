#include "TruckDepotController.h"
#include "Entity/Entity.h"
#include "Entity/EntityPresets/VehiclePresets/VehiclePresets.h"
#include "Game/Game.h"
#include "Component/Transform/Transform.h"
#include "Component/TransitStop/TransitStop.h"

TruckDepotController::TruckDepotController() : DepotController(TransitStop::TransitType::Car) {}
void TruckDepotController::spawnVehicleForRoute(TransitRoute route) {
	this->getEntity()->getGame()->addEntity(
		VehiclePresets::cargoTruck(
			this->getEntity()->getGame(),
			this->getEntity()->transitStop->getCarStop().tile,
			IsoRotation::NORTH,
			prepareRouteForVehicle(route),
			this->getEntity()
		)
	);
}

const char* TruckDepotController::getUiName() {
	return "Truck Depot";
}

