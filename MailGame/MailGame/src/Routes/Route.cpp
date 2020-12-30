#include "Route.h"

id_t Route::ROUTE_ID = 0;

SaveData routeToSaveData(Route r) {
	SaveData data("Route");
	data.addValue("departTime", r.departTime);
	data.addValue("vehicleModel", vehicleModelToString(r.model));
	if (r.cargoCarModel.has_value())
		data.addValue("cargoCarModel", cargoCarModelToString(r.cargoCarModel.value()));
	data.addValue("numCargoCars", r.numCargoCars);
	return data;
}

Route saveDataToRoute(SaveData data) {
	std::optional<CargoCarModel> car = data.hasValue("cargoCarModel") ? stringToCargoCarModel(data.getValue("cargoCarModel")) : std::optional<CargoCarModel>();
	return Route(
		(gtime_t)std::stoull(data.getValue("departTime")),
		stringToVehicleModel(data.getValue("vehicleModel")),
		car,
		(unsigned int)std::stoi(data.getValue("numCargoCars"))
	);
}