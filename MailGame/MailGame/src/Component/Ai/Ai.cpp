#include "Ai.h"
#include "Routes/Route.h"

void Ai::onArriveAtTile(sf::Vector2i tile) {}
void Ai::onArriveAtStop(size_t stopIndex) {}
void Ai::onArriveAtDest() {}

unsigned long long Ai::getCapacity(Route route) {
	VehicleModelInfo mInfo = VehicleModelInfo::getModelInfo(route.model);
	if (route.cargoCarModel.has_value()) {
		CargoCarInfo cInfo = CargoCarInfo::get(route.cargoCarModel.value());
		return mInfo.getCapacity() + route.numCargoCars * cInfo.getCapacity();
	}
	else {
		return mInfo.getCapacity();
	}
}