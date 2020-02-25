#include "TruckDepotController.h"
#include "Entity/Entity.h"
#include "Entity/EntityPresets/VehiclePresets/VehiclePresets.h"
#include "Game/Game.h"
#include "Component/Transform/Transform.h"

void TruckDepotController::spawnVehicleForRoute(TransitRoute route) {
	this->getEntity()->getGame()->addEntity(
		VehiclePresets::cargoTruck(
			this->getEntity()->getGame(),
			this->getEntity()->transform->getPosition()
			+ this->getEntity()->transform->getRotation().getUnitVector(),
			IsoRotation::NORTH,
			route,
			this->getEntity()
		)
	);
}

const char* TruckDepotController::getUiName() {
	return "Truck Depot";
}
