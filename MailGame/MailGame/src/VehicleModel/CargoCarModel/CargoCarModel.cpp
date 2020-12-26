#include "CargoCarModel.h"

CargoCarInfo::CargoCarInfo(std::string n, unsigned int c, float m) : name(n), capacity(c), maxSpeed(m) {}

#define PAIR(model, info) { model, info }
const std::map<CargoCarModel, CargoCarInfo> CargoCarInfo::INFOS = {
	PAIR(CargoCarModel::TrainCar, CargoCarInfo("Train Car", 100, 20.0f)),
	PAIR(CargoCarModel::BigTrainCar, CargoCarInfo("Large train car", 400, 10.0f)),
	PAIR(CargoCarModel::SmallTrainCar, CargoCarInfo("Small Train car", 50, 50.0f))
};
#undef PAIR

CargoCarInfo CargoCarInfo::get(CargoCarModel model) {
	return INFOS.find(model)->second;
}

std::string CargoCarInfo::getName() {
	return this->name;
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