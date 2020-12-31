#include "Route.h"
#include "System/SaveData/SaveData.h"

id_t Route::ROUTE_ID = 0;

Route::Route(hour_t departTime, VehicleModel model, std::optional<CargoCarModel> cargoCarModel, unsigned int numCargoCars)

	: departTime(departTime), model(model), cargoCarModel(cargoCarModel), numCargoCars(numCargoCars), id(ROUTE_ID++) {}
SaveData Route::getSaveData() {
	SaveData data("Route");
	data.addValue("departTime", this->departTime);
	data.addValue("vehicleModel", vehicleModelToString(this->model));
	if (this->cargoCarModel.has_value())
		data.addValue("cargoCarModel", cargoCarModelToString(this->cargoCarModel.value()));
	data.addValue("numCargoCars", this->numCargoCars);
	return data;
}

Route::Route(SaveData data)
	: Route((hour_t)std::stoull(data.getValue("departTime")),
		stringToVehicleModel(data.getValue("vehicleModel")),
		data.hasValue("cargoCarModel") ? stringToCargoCarModel(data.getValue("cargoCarModel")) : std::optional<CargoCarModel>(),
		(unsigned int)std::stoi(data.getValue("numCargoCars"))
	) {}