#include "TransitStop.h"
#include "System/SaveData/SaveData.h"
#include "Component/ComponentType/ComponentType.h"
#include "Entity/Entity.h"
#include <string>

// Currently this is just the speed that all planes taxi at
// Will it be plane individual or something? idk
const float TAXI_SPEED = 0.6f;

TransitStop::TransitStop(
	std::optional<CarStop> carStop,
	std::optional<TrainStop> trainStop,
	std::optional<AirplaneStop> airplaneStop
) : carStop(carStop), trainStop(trainStop), airplaneStop(airplaneStop) {}
TransitStop::CarStop TransitStop::getCarStop() {
	if (carStop.has_value()) {
		return carStop.value();
	}
	throw std::runtime_error("getRoadTransitLocation called but not implemented!");
}

TransitStop::TrainStop TransitStop::getTrainStop() {
	if (trainStop.has_value()) {
		return trainStop.value();
	}
	throw std::runtime_error("getRoadTransitLocation called but not implemented!");
}

TransitStop::AirplaneStop TransitStop::getAirplaneStop() {
	if (airplaneStop.has_value()) {
		return airplaneStop.value();
	}
	throw std::runtime_error("getRoadTransitLocation called but not implemented!");
}

bool TransitStop::getRunwayLock() {
	if (this->airplaneStop.value().isLocked) {
		return false;
	}
	this->airplaneStop.value().isLocked = true;
	return true;
}

void TransitStop::releaseRunwayLock() {
	this->airplaneStop.value().isLocked = false;
}
std::vector<SpeedPoint> TransitStop::getArrivingTransitPath(std::shared_ptr<Entity> e, TransitType type) {
	auto transit = e->transitStop;
	switch (type) {
	case TransitType::Car:
		return { transit->getCarStop().tile };
	case TransitType::Train:
		return { transit->getTrainStop().tile };
	case TransitType::Airplane:
		// Compute where we need to go
		auto aStop = transit->getAirplaneStop();
		auto dir = aStop.end - aStop.begin;
		// TBA add z
		sf::Vector3f unit = dir / (sqrt(dir.x * dir.x + dir.y * dir.y));
		// Th horizontal distance to cover when descending
		const float DESCENT_LENGTH = 5.0f;
		const float PLANE_HEIGHT = 4.0f;
		sf::Vector3f airStart = aStop.begin - DESCENT_LENGTH * unit;
		airStart.z = PLANE_HEIGHT;
		sf::Vector3f airEnd = aStop.end + unit * DESCENT_LENGTH;
		airEnd.z = PLANE_HEIGHT;
		sf::Vector3f halfway((aStop.end + aStop.depot) / 2.0f);
		return  { airStart, aStop.begin, SpeedPoint(aStop.end, 0.0f), SpeedPoint(halfway, TAXI_SPEED), SpeedPoint(aStop.depot, 0.0f) };
	}
	return {};
}
std::vector<SpeedPoint> TransitStop::getDepartingTransitPath(std::shared_ptr<Entity> e, TransitType type) {
	auto transit = e->transitStop;
	// TODO: Once there are better depot types/more defined railways and driveways, this will be more complicated
	switch (type) {
	case TransitType::Car:
	case TransitType::Train:
		// This is a hacky temporary shortcut
		return getArrivingTransitPath(e, type);
	case TransitType::Airplane:
		// Compute where we need to go
		auto aStop = transit->getAirplaneStop();
		auto dir = aStop.end - aStop.begin;
		// TBA add z
		sf::Vector3f unit = dir / (sqrt(dir.x * dir.x + dir.y * dir.y));
		// Th horizontal distance to cover when descending
		const float DESCENT_LENGTH = 5.0f;
		const float PLANE_HEIGHT = 4.0f;
		sf::Vector3f airStart = aStop.begin - DESCENT_LENGTH * unit;
		airStart.z = PLANE_HEIGHT;
		sf::Vector3f airEnd = aStop.end + unit * DESCENT_LENGTH;
		airEnd.z = PLANE_HEIGHT;
		sf::Vector3f halfway((aStop.begin + aStop.depot) / 2.0f);
		return  { SpeedPoint(aStop.depot, 0.0f), SpeedPoint(halfway, TAXI_SPEED), SpeedPoint(aStop.begin, 0.0f), aStop.end, airEnd };
	}
}

std::optional<SaveData> TransitStop::getSaveData() {
	using namespace SaveKeys;
	SaveData data(componentTypeToStr(ComponentType::TransitStop));
	if (carStop.has_value()) {
		SaveData carData(CAR_STOP);
		carData.addVector3f(POSITION, carStop.value().tile);
		if (carStop.value().dir.has_value())
			carData.addIsoRotation(ROTATION, carStop.value().dir.value());
		data.addData(carData);
	}
	if (trainStop.has_value()) {
		SaveData trainData(TRAIN_STOP);
		trainData.addVector3f(POSITION, trainStop.value().tile);
		trainData.addIsoRotation(ROTATION, trainStop.value().rot);
		data.addData(trainData);
	}
	if (airplaneStop.has_value()) {
		SaveData airplaneData(AIRPLANE_STOP);
		airplaneData.addVector3f(START, airplaneStop.value().begin);
		airplaneData.addVector3f(END, airplaneStop.value().end);
		airplaneData.addVector3f(DEPOT, airplaneStop.value().depot);
		data.addData(airplaneData);
	}
	return { data };
}

void TransitStop::fromSaveData(SaveData data) {
	using namespace SaveKeys;
	for (SaveData d : data.getDatas()) {
		if (d.getName() == AIRPLANE_STOP) {
			this->airplaneStop = {
				d.getVector3f(START),
				d.getVector3f(END),
				d.getVector3f(DEPOT)
			};
		}
		else if (d.getName() == TRAIN_STOP) {
			this->trainStop = {
				d.getVector3f(POSITION),
				d.getIsoRotation(ROTATION)
			};
		}
		else if (d.getName() == CAR_STOP) {
			std::optional<IsoRotation> dir;
			if (d.hasValue(ROTATION)) {
				dir = d.getIsoRotation(ROTATION);
			}
			this->carStop = {
				d.getVector3f(POSITION),
				dir
			};
		}
		else {
			throw std::runtime_error("Invalid SaveData name in TransitStop data");
		}
	}
}