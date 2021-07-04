#include "./BoatDepotController.h"

BoatDepotController::BoatDepotController() : DepotController(TransitType::Boat, { VehicleModel::SlowBoat, VehicleModel::FastBoat }) {}
void BoatDepotController::spawnVehicleForRoute(TransitRoute route) {
}

const char* BoatDepotController::getUiName() {
	return "Boat Depot";
}