#pragma once
#include <string>
#include <map>
#include "System/IsoSprite/IsoSprite.h"
/*
 * A CargoCar is a vehicle attachment that holds cargo
 * i.e. a train car, or the trailer to an 18 wheeler
 */

#define CARGO_CARS \
    f(TrainCar) \
    f(BigTrainCar) \
    f(SmallTrainCar)

#define f(x) x,

enum class CargoCarModel {
	CARGO_CARS
};

#undef f
#define f(x) { CargoCarModel::x, #x },

const std::map<CargoCarModel, std::string> CARGO_CAR_STRINGS = {
	CARGO_CARS
};

#undef f
#undef CARGO_CARS

class CargoCarInfo {
public:
	CargoCarInfo(std::string name, unsigned int capacity, float maxSpeed, IsoSprite sprites);
	static CargoCarInfo get(CargoCarModel model);

	std::string getName();
	IsoSprite getSprites();
private:
	static const std::map<CargoCarModel, CargoCarInfo> INFOS;

	std::string name;
	unsigned int capacity;
	float maxSpeed;
	IsoSprite sprites;
};

std::string cargoCarModelToString(CargoCarModel car);
CargoCarModel stringToCargoCarModel(std::string s);
