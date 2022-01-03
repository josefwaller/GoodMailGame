#include "Component/Controller/DepotController/PlaneDepotController/PlaneDepotController.h"
#include "Entity/Entity.h"
#include "Game/Game.h"
#include "Component/Transform/Transform.h"
#include "VehicleModel/VehicleModel.h"
#include "System/Utils/Utils.h"

PlaneDepotController::PlaneDepotController() : DepotController(TransitType::Airplane, { VehicleModel::SlowAirplane, VehicleModel::FastAirplane }) {}

void PlaneDepotController::spawnVehicleForRoute(TransitRoute route) {
	gtime_t spawnTime = this->getEntity()->getGame()->getMidnightTime() + Game::UNITS_IN_GAME_HOUR * route.departTime;
	this->getEntity()->getGame()->addEntity(
		VehiclePresets::plane(
			this->getEntity()->getGame(),
			this->getEntity()->transform->getPosition(),
			this->getEntity()->transform->getRotation(),
			prepareRouteForVehicle(route),
			this->getEntity(),
			spawnTime
		)
	);
}
const char* PlaneDepotController::getUiName() {
	return "Airport";
}

void PlaneDepotController::resetRouteLength(size_t routeId) {

}