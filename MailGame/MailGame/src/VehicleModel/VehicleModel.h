#pragma once
#include <map>
#include <string>
#include <vector>
#include "Component/TransitStop/TransitStop.h"
#include "CargoCarModel/CargoCarModel.h"
#include "Constants.h"
#include "TechTree/Technology/Technology.h"

// The different Vehicle Models in the game
#define Models \
	f(MailTruck) \
	f(SlowCargoTruck) \
	f(FastCargoTruck) \
	f(SlowTrain) \
	f(FastTrain) \
	f(SlowAirplane) \
	f(FastAirplane) \
	f(SlowBoat) \
	f(FastBoat)

#define f(x) x,

enum class VehicleModel {
	Models
};

#undef f
#define f(x) { VehicleModel::x, #x },

const std::vector<std::pair<VehicleModel, std::string>> modelStrs = {
	Models
};

#undef f
#undef Models
// The information about each vehicle model
class VehicleModelInfo {
public:
	static VehicleModelInfo getModelInfo(VehicleModel model);
	VehicleModelInfo(
		float speed,
		gtime_t loadTime,
		gtime_t unloadTime,
		std::string name,
		unsigned int capacity,
		Technology requiredTech = Technology::Default,
		money_t initialCost = 100,
		money_t costPerTile = 1,
		std::vector<CargoCarModel> allowedCargoCars = {},
		unsigned int maxNumCargoCars = 0);
	float getSpeed();
	float getAcceleration();
	gtime_t getLoadTime();
	gtime_t getUnloadTime();
	std::string getName();
	Technology getRequiredTechnology();
	money_t getInitialCost();
	money_t getCostPerTile();
	std::vector<CargoCarModel> getAllowedCargoCars();
	unsigned int getMaxNumCargoCars();
	unsigned int getCapacity();
private:
	const static std::map<VehicleModel, VehicleModelInfo> modelInfos;
	// The speed the vehicle moves at
	float speed;
	// The time it takes to load/unload the vehicle
	gtime_t loadTime;
	gtime_t unloadTime;
	std::string name;
	Technology requiredTechnology;
	money_t initialCost;
	money_t costPerTile;
	std::vector<CargoCarModel> allowedCargoCars;
	unsigned int maxNumCargoCars;
	unsigned int capacity;
};

// Used for saving/loading
std::string vehicleModelToString(VehicleModel v);
VehicleModel stringToVehicleModel(std::string s);
