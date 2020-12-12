#include "VehicleModel.h"
#include "Game/Game.h"

VehicleModelInfo::VehicleModelInfo(float s, gtime_t l, gtime_t u, std::string n, Technology t, money_t c) : speed(s), loadTime(l), unloadTime(u), name(n), requiredTechnology(t), cost(c) {}

float VehicleModelInfo::getSpeed() { return this->speed; }
gtime_t VehicleModelInfo::getLoadTime() { return this->loadTime; }
gtime_t VehicleModelInfo::getUnloadTime() { return this->unloadTime; }
std::string VehicleModelInfo::getName() { return this->name; }
Technology VehicleModelInfo::getRequiredTechnology() { return this->requiredTechnology; }

// These will eventually be actual models of cars/trains/etc, but for now they're just types
const std::map<VehicleModel, VehicleModelInfo> VehicleModelInfo::modelInfos = {
	{ VehicleModel::MailTruck, VehicleModelInfo(1.0f, Game::UNITS_IN_GAME_HOUR * 0.2, Game::UNITS_IN_GAME_HOUR * 0.2, "Mail Truck", Technology::Default, 20) },
	{ VehicleModel::SlowCargoTruck, VehicleModelInfo(1.0f, Game::UNITS_IN_GAME_HOUR * 0.2, Game::UNITS_IN_GAME_HOUR * 0.2, "Cargo Truck", Technology::CargoTrucks) },
	{ VehicleModel::FastCargoTruck, VehicleModelInfo(1.5f, Game::UNITS_IN_GAME_HOUR * 1.2, Game::UNITS_IN_GAME_HOUR * 2, "Fast Truck", Technology::FastCargoTrucks) },
	{ VehicleModel::SlowTrain, VehicleModelInfo(1.0f, Game::UNITS_IN_GAME_HOUR * 0.2, Game::UNITS_IN_GAME_HOUR * 0.2, "Slow Train", Technology::Trains) },
	{ VehicleModel::FastTrain, VehicleModelInfo(3.5f, Game::UNITS_IN_GAME_HOUR * 0.2, Game::UNITS_IN_GAME_HOUR * 0.2, "Fast Train", Technology::FastTrains) },
	{ VehicleModel::SlowAirplane, VehicleModelInfo(5.0f, Game::UNITS_IN_GAME_HOUR * 0.2, Game::UNITS_IN_GAME_HOUR * 0.2, "Slow Plane", Technology::Airplanes) },
	{ VehicleModel::FastAirplane, VehicleModelInfo(6.0f, Game::UNITS_IN_GAME_HOUR * 0.2, Game::UNITS_IN_GAME_HOUR * 0.2, "Fast Plane", Technology::FastAirplanes) }
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

money_t VehicleModelInfo::getCostPerRoute() {
	return this->cost;
}