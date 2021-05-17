#include "CargoCarModel.h"
#include "ResourceLoader/ResourceLoader.h"

CargoCarInfo::CargoCarInfo(std::string n, unsigned int c, float m, IsoSprite s) : name(n), capacity(c), maxSpeed(m), sprites(s) {}

#define PAIR(model, info) { model, info }
const std::map<CargoCarModel, CargoCarInfo> CargoCarInfo::INFOS = {
	PAIR(CargoCarModel::TrainCar, CargoCarInfo("Train Car", 100, 20.0f, IsoSprite(
		ResourceLoader::get()->getSprite("vehicles/vehicles", "trainCar-reg-N.png", true),
		ResourceLoader::get()->getSprite("vehicles/vehicles", "trainCar-reg-E.png", true),
		ResourceLoader::get()->getSprite("vehicles/vehicles", "trainCar-reg-W.png", true),
		ResourceLoader::get()->getSprite("vehicles/vehicles", "trainCar-reg-S.png", true)
	))),
	PAIR(CargoCarModel::BigTrainCar, CargoCarInfo("Large train car", 400, 10.0f, IsoSprite(
		ResourceLoader::get()->getSprite("vehicles/vehicles", "trainCar-large-N.png", true),
		ResourceLoader::get()->getSprite("vehicles/vehicles", "trainCar-large-NE.png", true),
		ResourceLoader::get()->getSprite("vehicles/vehicles", "trainCar-large-E.png", true),
		ResourceLoader::get()->getSprite("vehicles/vehicles", "trainCar-large-SE.png", true),
		ResourceLoader::get()->getSprite("vehicles/vehicles", "trainCar-large-S.png", true),
		ResourceLoader::get()->getSprite("vehicles/vehicles", "trainCar-large-SW.png", true),
		ResourceLoader::get()->getSprite("vehicles/vehicles", "trainCar-large-W.png", true),
		ResourceLoader::get()->getSprite("vehicles/vehicles", "trainCar-large-NW.png", true)
	))),
	PAIR(CargoCarModel::SmallTrainCar, CargoCarInfo("Small Train car", 50, 50.0f, IsoSprite(
		ResourceLoader::get()->getSprite("vehicles/vehicles", "trainCar-small-N.png", true),
		ResourceLoader::get()->getSprite("vehicles/vehicles", "trainCar-small-E.png", true),
		ResourceLoader::get()->getSprite("vehicles/vehicles", "trainCar-small-W.png", true),
		ResourceLoader::get()->getSprite("vehicles/vehicles", "trainCar-small-S.png", true)
	)))
};
#undef PAIR

CargoCarInfo CargoCarInfo::get(CargoCarModel model) {
	return INFOS.find(model)->second;
}

std::string CargoCarInfo::getName() {
	return this->name;
}
IsoSprite CargoCarInfo::getSprites() {
	return this->sprites;
}
unsigned int CargoCarInfo::getCapacity() {
	return this->capacity;
}

CargoCarModel stringToCargoCarModel(std::string s) {
	for (auto it = CARGO_CAR_STRINGS.begin(); it != CARGO_CAR_STRINGS.end(); it++) {
		if (it->second == s) {
			return it->first;
		}
	}
	return CargoCarModel::TrainCar;
}
std::string cargoCarModelToString(CargoCarModel car) {
	return CARGO_CAR_STRINGS.find(car)->second;
}