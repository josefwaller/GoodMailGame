#include "VehicleModel.h"
#include "Game/Game.h"

VehicleModelInfo::VehicleModelInfo(float s, gtime_t l, gtime_t u, std::string n, unsigned int capacity, Technology t, money_t c, std::vector<CargoCarModel> cC, unsigned int mCc)
	: speed(s),
	loadTime(l),
	unloadTime(u),
	name(n),
	requiredTechnology(t),
	cost(c),
	allowedCargoCars(cC),
	maxNumCargoCars(mCc),
	capacity(capacity) {}

float VehicleModelInfo::getSpeed() { return this->speed; }
// TODO
float VehicleModelInfo::getAcceleration() { return 150.0f; };
gtime_t VehicleModelInfo::getLoadTime() { return this->loadTime; }
gtime_t VehicleModelInfo::getUnloadTime() { return this->unloadTime; }
std::string VehicleModelInfo::getName() { return this->name; }
Technology VehicleModelInfo::getRequiredTechnology() { return this->requiredTechnology; }
unsigned int VehicleModelInfo::getMaxNumCargoCars() { return this->maxNumCargoCars; }
unsigned int VehicleModelInfo::getCapacity() {
	return this->capacity;
};

const std::vector<CargoCarModel> TRAIN_CARS = {
	CargoCarModel::TrainCar,
	CargoCarModel::BigTrainCar,
	CargoCarModel::SmallTrainCar,
};

// These will eventually be actual models of cars/trains/etc, but for now they're just types
const std::map<VehicleModel, VehicleModelInfo> VehicleModelInfo::modelInfos = {
	{ VehicleModel::MailTruck, VehicleModelInfo(25.0f, Game::UNITS_IN_GAME_HOUR * 0.2, Game::UNITS_IN_GAME_HOUR * 0.2, "Mail Truck", 200, Technology::Default, 20) },
	{ VehicleModel::SlowCargoTruck, VehicleModelInfo(25.0f, Game::UNITS_IN_GAME_HOUR * 0.2, Game::UNITS_IN_GAME_HOUR * 0.2, "Cargo Truck", 600, Technology::CargoTrucks) },
	{ VehicleModel::FastCargoTruck, VehicleModelInfo(45.0f, Game::UNITS_IN_GAME_HOUR * 1.2, Game::UNITS_IN_GAME_HOUR * 2, "Fast Truck", 600, Technology::FastCargoTrucks) },
	{ VehicleModel::SlowTrain, VehicleModelInfo(100.0f, Game::UNITS_IN_GAME_HOUR * 0.2, Game::UNITS_IN_GAME_HOUR * 0.2, "Slow Train", 400, Technology::Trains, 200, TRAIN_CARS, 8) },
	{ VehicleModel::FastTrain, VehicleModelInfo(150.0f, Game::UNITS_IN_GAME_HOUR * 0.2, Game::UNITS_IN_GAME_HOUR * 0.2, "Fast Train", 400, Technology::FastTrains, 400, TRAIN_CARS, 4) },
	{ VehicleModel::SlowAirplane, VehicleModelInfo(300.0f, Game::UNITS_IN_GAME_HOUR * 0.2, Game::UNITS_IN_GAME_HOUR * 0.2, "Slow Plane", 800, Technology::Airplanes) },
	{ VehicleModel::FastAirplane, VehicleModelInfo(400.0f, Game::UNITS_IN_GAME_HOUR * 0.2, Game::UNITS_IN_GAME_HOUR * 0.2, "Fast Plane", 800, Technology::FastAirplanes) },
	{ VehicleModel::SlowBoat, VehicleModelInfo(100.0f, Game::UNITS_IN_GAME_HOUR * 0.2, Game::UNITS_IN_GAME_HOUR * 0.2, "Slow Boat", 2000, Technology::Default) },
	{ VehicleModel::FastBoat, VehicleModelInfo(200.0f, Game::UNITS_IN_GAME_HOUR * 0.2, Game::UNITS_IN_GAME_HOUR * 0.2, "Fast Boat", 2000, Technology::Default) }
};

VehicleModelInfo VehicleModelInfo::getModelInfo(VehicleModel m) {
	return VehicleModelInfo::modelInfos.at(m);
}

std::vector<CargoCarModel> VehicleModelInfo::getAllowedCargoCars() {
	return this->allowedCargoCars;
}

std::string vehicleModelToString(VehicleModel v) {
	for (auto kv : modelStrs) {
		if (kv.first == v) {
			return kv.second;
		}
	}
	return "Missing VehicleModel string";
}

VehicleModel stringToVehicleModel(std::string s) {
	for (auto kv : modelStrs) {
		if (kv.second == s) {
			return kv.first;
		}
	}
	throw std::runtime_error("Missing VehicleModel for string " + s);
}

money_t VehicleModelInfo::getCostPerRoute() {
	return this->cost;
}