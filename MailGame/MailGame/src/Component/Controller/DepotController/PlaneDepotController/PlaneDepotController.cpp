#include "Component/Controller/DepotController/PlaneDepotController/PlaneDepotController.h"
#include "Entity/Entity.h"
#include "Game/Game.h"
#include "Component/Transform/Transform.h"

PlaneDepotController::PlaneDepotController() : DepotController(TransitStop::TransitType::Airplane) {}

void PlaneDepotController::spawnVehicleForRoute(TransitRoute route) {
	this->getEntity()->getGame()->addEntity(
		VehiclePresets::plane(
			this->getEntity()->getGame(),
			this->getEntity()->transform->getPosition(),
			this->getEntity()->transform->getRotation(),
			prepareRouteForVehicle(route),
			this->getEntity()
		)
	);
}
const char* PlaneDepotController::getUiName() {
	return "Airport";
}
