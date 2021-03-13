#include "Route.h"
#include "System/SaveData/SaveData.h"

id_t Route::ROUTE_ID = 0;

Route::Route(hour_t departTime, VehicleModel model, std::optional<CargoCarModel> cargoCarModel, unsigned int numCargoCars)

	: departTime(departTime), model(model), cargoCarModel(cargoCarModel), numCargoCars(numCargoCars), id(ROUTE_ID++) {}
SaveData Route::getSaveData() {
	using namespace SaveKeys;
	SaveData data(ROUTE);
	data.addHourT(DEPART_TIME, this->departTime);
	data.addString(VEHICLE_MODEL, vehicleModelToString(this->model));
	if (this->cargoCarModel.has_value())
		data.addString(CARGO_CAR_MODEL, cargoCarModelToString(this->cargoCarModel.value()));
	data.addSizeT(NUM_CARGO_CARS, this->numCargoCars);
	return data;
}

Route::Route(SaveData data)
	: Route(data.getHourT(SaveKeys::DEPART_TIME),
		stringToVehicleModel(data.getString(SaveKeys::VEHICLE_MODEL)),
		data.hasValue(SaveKeys::CARGO_CAR_MODEL) ? stringToCargoCarModel(data.getString(SaveKeys::CARGO_CAR_MODEL)) : std::optional<CargoCarModel>(),
		(unsigned int)data.getSizeT(SaveKeys::NUM_CARGO_CARS)
	) {}