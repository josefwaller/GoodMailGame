#include "Route.h"
#include "System/SaveData/SaveData.h"

id_t Route::ROUTE_ID = 0;

Route::Route(gtime_t departTime, VehicleModel model, std::optional<CargoCarModel> cargoCarModel, unsigned int numCargoCars)

	: departTime(departTime), model(model), cargoCarModel(cargoCarModel), numCargoCars(numCargoCars), id(ROUTE_ID++) {}
SaveData Route::getSaveData() {
	using namespace SaveKeys;
	SaveData data(ROUTE);
	data.addHourT(DEPART_TIME, this->departTime);
	data.addString(VEHICLE_MODEL, vehicleModelToString(this->model));
	if (this->cargoCarModel.has_value())
		data.addString(CARGO_CAR_MODEL, cargoCarModelToString(this->cargoCarModel.value()));
	data.addSizeT(NUM_CARGO_CARS, this->numCargoCars);
	data.addFloat(LENGTH, this->length);
	return data;
}

Route::Route(SaveData data)
	: departTime(data.getGTimeT(SaveKeys::DEPART_TIME)),
		model(stringToVehicleModel(data.getString(SaveKeys::VEHICLE_MODEL))),
		cargoCarModel(data.hasValue(SaveKeys::CARGO_CAR_MODEL) ? stringToCargoCarModel(data.getString(SaveKeys::CARGO_CAR_MODEL)) : std::optional<CargoCarModel>()),
		numCargoCars((unsigned int)data.getSizeT(SaveKeys::NUM_CARGO_CARS)),
		length(data.getFloat(SaveKeys::LENGTH)),
		id(ROUTE_ID++) {}