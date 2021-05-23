#include "TrainDepotController.h"
#include "Entity/Entity.h"
#include "Game/Game.h"
#include "Component/Transform/Transform.h"
#include "Component/TransitStop/TransitStop.h"
#include "Entity/EntityPresets/VehiclePresets/VehiclePresets.h"

TrainDepotController::TrainDepotController() : DepotController(TransitType::Train, { VehicleModel::SlowTrain, VehicleModel::FastTrain }) {}

void TrainDepotController::spawnVehicleForRoute(TransitRoute route) {
	gtime_t spawnTime = this->getEntity()->getGame()->getMidnightTime() + Game::UNITS_IN_GAME_HOUR * route.departTime;
	this->getEntity()->getGame()->addEntity(
		VehiclePresets::train(
			this->getEntity()->getGame(),
			this->getEntity()->transitStop->getTrainStop().tile,
			this->getEntity()->transform->getRotation().getReverse(),
			prepareRouteForVehicle(route),
			this->getEntity(),
			spawnTime
		)
	);
}
const char* TrainDepotController::getUiName() {
	return "Train Depot";
}