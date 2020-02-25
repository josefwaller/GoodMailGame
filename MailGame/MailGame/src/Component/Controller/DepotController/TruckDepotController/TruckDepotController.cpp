#include "TruckDepotController.h"
#include "Entity/Entity.h"
#include "Entity/EntityPresets/EntityPresets.h"
#include "Game/Game.h"
#include "Component/Transform/Transform.h"

void TruckDepotController::spawnVehicleForRoute(TransitRoute route) {
	this->getEntity()->getGame()->addEntity(
		EntityPresets::cargoTruck(
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
