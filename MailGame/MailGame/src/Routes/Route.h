#pragma once
#include "Constants.h"
#include "VehicleModel/VehicleModel.h"
#include "VehicleModel/CargoCarModel/CargoCarModel.h"

class SaveData;

class Route {
public:
	// The dparture time of the route
	hour_t departTime;
	// The model of the vehicle on the route
	VehicleModel model;
	// Optionally, the model and number of cargo cars
	std::optional<CargoCarModel> cargoCarModel;
	unsigned int numCargoCars;
	// The id
	id_t id;
	static id_t ROUTE_ID;
	// Constructor
	Route(hour_t departTime, VehicleModel model, std::optional<CargoCarModel> cargoCarModel = std::optional<CargoCarModel>(), unsigned int numCargoCars = 0);
	// Constructor from save data
	Route(SaveData data);
	// Get save data
	SaveData getSaveData();
	// The length of the route
	float length;
};
