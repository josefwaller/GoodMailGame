#pragma once
#include <string>
#include <map>
/*
 * A CargoCar is a vehicle attachment that holds cargo
 * i.e. a train car, or the trailer to an 18 wheeler
 */

#define CARGO_CARS \
    f(TrainCar)

#define f(x) x,

enum class CargoCarModel {
    CARGO_CARS
};

#undef f
#define f(x) { CargoCarModel::x, #x }

const std::map<CargoCarModel, std::string> CARGO_CAR_STRINGS = {
    CARGO_CARS
};

#undef f
#undef CARGO_CARS

class CargoCarInfo {
public:
    CargoCarInfo(unsigned int capacity, float maxSpeed);
private:
    unsigned int capacity;
    float maxSpeed;
};

std::string cargoCarModelToString(CargoCarModel car);
CargoCarModel stringToCargoCarModel(std::string s);
