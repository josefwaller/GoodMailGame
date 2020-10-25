#include "VehicleModel.h"
#include "Game/Game.h"

VehicleModelInfo::VehicleModelInfo(float s, gtime_t l, gtime_t u, std::string n) : speed(s), loadTime(l), unloadTime(u), name(n) {}

float VehicleModelInfo::getSpeed() { return this->speed; }
gtime_t VehicleModelInfo::getLoadTime() { return this->loadTime; }
gtime_t VehicleModelInfo::getUnloadTime() { return this->unloadTime; }
std::string VehicleModelInfo::getName() { return this->name; }

// These will eventually be actual models of cars/trains/etc, but for now they're just types
const std::map<VehicleModel, VehicleModelInfo> VehicleModelInfo::modelInfos = {
	{ VehicleModel::MailTruck, VehicleModelInfo(1.0f, Game::UNITS_IN_GAME_HOUR * 0.2, Game::UNITS_IN_GAME_HOUR * 0.2, "Mail Truck") },
	{ VehicleModel::SlowCargoTruck, VehicleModelInfo(1.0f, Game::UNITS_IN_GAME_HOUR * 0.2, Game::UNITS_IN_GAME_HOUR * 0.2, "Slow Truck") },
	{ VehicleModel::FastCargoTruck, VehicleModelInfo(1.5f, Game::UNITS_IN_GAME_HOUR * 0.2, Game::UNITS_IN_GAME_HOUR * 0.2, "Fast Truck") },
	{ VehicleModel::SlowTrain, VehicleModelInfo(1.0f, Game::UNITS_IN_GAME_HOUR * 0.2, Game::UNITS_IN_GAME_HOUR * 0.2, "Slow Train") },
	{ VehicleModel::FastTrain, VehicleModelInfo(3.5f, Game::UNITS_IN_GAME_HOUR * 0.2, Game::UNITS_IN_GAME_HOUR * 0.2, "Fast Train") },
	{ VehicleModel::SlowAirplane, VehicleModelInfo(5.0f, Game::UNITS_IN_GAME_HOUR * 0.2, Game::UNITS_IN_GAME_HOUR * 0.2, "Slow Plane") },
	{ VehicleModel::FastAirplane, VehicleModelInfo(6.0f, Game::UNITS_IN_GAME_HOUR * 0.2, Game::UNITS_IN_GAME_HOUR * 0.2, "Fast Plane") }
};

VehicleModelInfo VehicleModelInfo::getModelInfo(VehicleModel m) {
	return VehicleModelInfo::modelInfos.at(m);
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