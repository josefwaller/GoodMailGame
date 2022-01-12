#include "./BoatDepotController.h"
#include "Entity/Entity.h"
#include "Game/Game.h"
#include "Component/Transform/Transform.h"
#include "Entity/EntityPresets/VehiclePresets/VehiclePresets.h"

BoatDepotController::BoatDepotController() : DepotController(TransitType::Boat, { VehicleModel::SlowBoat, VehicleModel::FastBoat }) {}
void BoatDepotController::spawnVehicleForRoute(TransitRoute route) {
	gtime_t spawnTime = this->getEntity()->getGame()->getMidnightTime() + route.departTime;
	this->getEntity()->getGame()->addEntity(VehiclePresets::boat(
		this->getEntity()->getGame(),
		this->getEntity()->transform->getPosition(),
		this->getEntity()->transform->getRotation(),
		route,
		this->getEntity(),
		spawnTime
	));
}

const char* BoatDepotController::getUiName() {
	return "Boat Depot";
}

void BoatDepotController::resetRouteLength(size_t routeIndex) {

}