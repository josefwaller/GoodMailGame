#include "TrainDepotController.h"
#include "Entity/Entity.h"
#include "Game/Game.h"
#include "Component/Transform/Transform.h"
#include "Entity/EntityPresets/EntityPresets.h"

void TrainDepotController::spawnVehicleForRoute(TransitRoute route) {
	this->getEntity()->getGame()->addEntity(
		EntityPresets::train(
			this->getEntity()->getGame(),
			this->getEntity()->transform->getPosition(),
			this->getEntity()->transform->getRotation(),
			route,
			this->getEntity()
		)
	);
}
const char* TrainDepotController::getUiName() {
	return "Train Depot";
}
