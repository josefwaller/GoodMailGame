#pragma once
#include "Constants.h"
#include "VehicleModel/VehicleModel.h"
#include "VehicleModel/CargoCarModel/CargoCarModel.h"
#include "System/SaveData/SaveData.h"

struct Route {
	// The dparture time of the route
	gtime_t departTime;
	// The model of the vehicle on the route
	VehicleModel model;
	// Optionally, the model and number of cargo cars
	std::optional<CargoCarModel> cargoCarModel;
	unsigned int numCargoCars;
	// The id
	id_t id;
	static id_t ROUTE_ID;
	// Constructor
	Route(gtime_t departTime, VehicleModel model, std::optional<CargoCarModel> cargoCarModel = {}, unsigned int numCargoCars = 0)
		: departTime(departTime), model(model), cargoCarModel(cargoCarModel), numCargoCars(numCargoCars), id(ROUTE_ID++) {}
};

SaveData routeToSaveData(Route route);
Route saveDataToRoute(SaveData data);
