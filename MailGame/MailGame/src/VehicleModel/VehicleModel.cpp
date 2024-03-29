#include "VehicleModel.h"
#include "Game/Game.h"

VehicleModelInfo::VehicleModelInfo(float s, gtime_t l, gtime_t u, std::string n, unsigned int capacity, Technology t, money_t initCost, money_t costPerTile, std::vector<CargoCarModel> cC, unsigned int mCc)
	: speed(s),
	loadTime(l),
	unloadTime(u),
	name(n),
	requiredTechnology(t),
	initialCost(initCost),
	costPerTile(costPerTile),
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
	{ VehicleModel::MailTruck, VehicleModelInfo(25.0f, Game::UNITS_IN_GAME_HOUR * 0.2, Game::UNITS_IN_GAME_HOUR * 0.2, "Mail Truck", 100, Technology::Default, 20, 1) },
	{ VehicleModel::SlowCargoTruck, VehicleModelInfo(25.0f, Game::UNITS_IN_GAME_HOUR * 0.2, Game::UNITS_IN_GAME_HOUR * 0.2, "Cargo Truck", 100, Technology::CargoTrucks, 100, 2) },
	{ VehicleModel::FastCargoTruck, VehicleModelInfo(45.0f, Game::UNITS_IN_GAME_HOUR * 1.2, Game::UNITS_IN_GAME_HOUR * 2, "Fast Truck", 100, Technology::FastCargoTrucks, 100, 4) },
	{ VehicleModel::SlowTrain, VehicleModelInfo(20.0f, Game::UNITS_IN_GAME_HOUR * 0.2, Game::UNITS_IN_GAME_HOUR * 0.2, "Slow Train", 100, Technology::Trains, 200, 1, TRAIN_CARS, 8) },
	{ VehicleModel::FastTrain, VehicleModelInfo(40.0f, Game::UNITS_IN_GAME_HOUR * 0.2, Game::UNITS_IN_GAME_HOUR * 0.2, "Fast Train", 100, Technology::FastTrains, 400, 1, TRAIN_CARS, 4) },
	{ VehicleModel::SlowAirplane, VehicleModelInfo(300.0f, Game::UNITS_IN_GAME_HOUR * 0.2, Game::UNITS_IN_GAME_HOUR * 0.2, "Slow Plane", 100, Technology::Airplanes, 100, 10) },
	{ VehicleModel::FastAirplane, VehicleModelInfo(400.0f, Game::UNITS_IN_GAME_HOUR * 0.2, Game::UNITS_IN_GAME_HOUR * 0.2, "Fast Plane", 100, Technology::FastAirplanes, 200, 15) },
/*	{ VehicleModel::SlowBoat, VehicleModelInfo(10.0f, Game::UNITS_IN_GAME_HOUR * 0.2, Game::UNITS_IN_GAME_HOUR * 0.2, "Slow Boat", 1000, Technology::Default) },
	{ VehicleModel::FastBoat, VehicleModelInfo(40.0f, Game::UNITS_IN_GAME_HOUR * 0.2, Game::UNITS_IN_GAME_HOUR * 0.2, "Fast Boat", 1000, Technology::Default) }*/
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

money_t VehicleModelInfo::getInitialCost() {
	return this->initialCost;
}
money_t VehicleModelInfo::getCostPerTile() {
	return this->costPerTile;
}