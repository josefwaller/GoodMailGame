#pragma once
#include "Constants.h"
#include "VehicleModel/VehicleModel.h"
#include "VehicleModel/CargoCarModel/CargoCarModel.h"

class SaveData;

class Route {
public:
	// The departure time of this route, where 0 = midnight and Game::UNITS_IN_GAME_HOUR * n = n o'clock
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
	Route(gtime_t departTime, VehicleModel model, std::optional<CargoCarModel> cargoCarModel = std::optional<CargoCarModel>(), unsigned int numCargoCars = 0);
	// Constructor from save data
	Route(SaveData data);
	// Get save data
	SaveData getSaveData();
	// The length of the route
	float length;
};
