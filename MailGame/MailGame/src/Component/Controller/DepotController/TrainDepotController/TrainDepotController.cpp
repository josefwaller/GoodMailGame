#include "TrainDepotController.h"
#include "Entity/Entity.h"
#include "Game/Game.h"
#include "Component/Transform/Transform.h"
#include "Component/TransitStop/TransitStop.h"
#include "Entity/EntityPresets/VehiclePresets/VehiclePresets.h"

TrainDepotController::TrainDepotController() : DepotController(TransitType::Train, { VehicleModel::SlowTrain, VehicleModel::FastTrain }) {}

void TrainDepotController::spawnVehicleForRoute(TransitRoute route) {
	this->getEntity()->getGame()->addEntity(
		VehiclePresets::train(
			this->getEntity()->getGame(),
			this->getEntity()->transitStop->getTrainStop().tile,
			this->getEntity()->transform->getRotation() + 1,
			prepareRouteForVehicle(route),
			this->getEntity(),
			route.departTime
		)
	);
}
const char* TrainDepotController::getUiName() {
	return "Train Depot";
}