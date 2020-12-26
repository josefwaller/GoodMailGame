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
	f(FastAirplane)

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
		Technology requiredTech = Technology::Default,
		money_t money = 100,
		std::vector<CargoCarModel> allowedCargoCars = {});
	float getSpeed();
	gtime_t getLoadTime();
	gtime_t getUnloadTime();
	std::string getName();
	Technology getRequiredTechnology();
	money_t getCostPerRoute();
	std::vector<CargoCarModel> getAllowedCargoCars();
private:
	const static std::map<VehicleModel, VehicleModelInfo> modelInfos;
	// The speed the vehicle moves at
	float speed;
	// The time it takes to load/unload the vehicle
	gtime_t loadTime;
	gtime_t unloadTime;
	std::string name;
	Technology requiredTechnology;
	money_t cost;
	std::vector<CargoCarModel> allowedCargoCars;
};

// Used for saving/loading
std::string vehicleModelToString(VehicleModel v);
VehicleModel stringToVehicleModel(std::string s);
