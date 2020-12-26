#include "CargoCarModel.h"

CargoCarInfo::CargoCarInfo(unsigned int c, float m) : capacity(c), maxSpeed(m) {}

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