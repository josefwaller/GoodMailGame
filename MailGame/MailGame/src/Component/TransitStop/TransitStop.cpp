#include "TransitStop.h"
#include "System/SaveData/SaveData.h"
#include "Component/ComponentType/ComponentType.h"
#include "Entity/Entity.h"
#include <string>

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
std::vector<sf::Vector3f> TransitStop::getArrivingTransitPath(std::shared_ptr<Entity> e, TransitStop::TransitType type) {
	auto transit = e->transitStop;
	switch (type) {
	case TransitStop::TransitType::Car:
		return { transit->getCarStop().tile };
	case TransitStop::TransitType::Train:
		return { transit->getTrainStop().tile };
	case TransitStop::TransitType::Airplane:
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
		return  { airStart, aStop.begin, aStop.end, aStop.depot };
	}
	return {};
}
std::vector<sf::Vector3f> TransitStop::getDepartingTransitPath(std::shared_ptr<Entity> e, TransitStop::TransitType type) {
	auto transit = e->transitStop;
	// TODO: Once there are better depot types/more defined railways and driveways, this will be more complicated
	switch (type) {
	case TransitStop::TransitType::Car:
	case TransitStop::TransitType::Train:
		// This is a hacky temporary shortcut
		return getArrivingTransitPath(e, type);
	case TransitStop::TransitType::Airplane:
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
		return  { aStop.depot, aStop.begin, aStop.end, airEnd };
	}
}

std::optional<SaveData> TransitStop::getSaveData() {
	SaveData data(componentTypeToStr(ComponentType::TransitStop));
	if (carStop.has_value()) {
		SaveData carData("CarStop");
		carData.addValue("posX", carStop.value().tile.x);
		carData.addValue("posY", carStop.value().tile.y);
		carData.addValue("posZ", carStop.value().tile.z);
		if (carStop.value().dir.has_value())
			carData.addValue("dir", carStop.value().dir.value().getRotation());
		data.addData(carData);
	}
	if (trainStop.has_value()) {
		SaveData trainData("TrainStop");
		trainData.addValue("posX", trainStop.value().tile.x);
		trainData.addValue("posY", trainStop.value().tile.y);
		trainData.addValue("posZ", trainStop.value().tile.z);
		trainData.addValue("rot", trainStop.value().rot.getRotation());
		data.addData(trainData);
	}
	if (airplaneStop.has_value()) {
		SaveData airplaneData("AirplaneStop");
		airplaneData.addValue("startX", airplaneStop.value().begin.x);
		airplaneData.addValue("startY", airplaneStop.value().begin.y);
		airplaneData.addValue("startZ", airplaneStop.value().begin.z);
		airplaneData.addValue("endX", airplaneStop.value().end.x);
		airplaneData.addValue("endY", airplaneStop.value().end.y);
		airplaneData.addValue("endZ", airplaneStop.value().end.z);
		airplaneData.addValue("depotX", airplaneStop.value().depot.x);
		airplaneData.addValue("depotY", airplaneStop.value().depot.y);
		data.addData(airplaneData);
	}
	return { data };
}

void TransitStop::fromSaveData(SaveData data) {
	for (SaveData d : data.getDatas()) {
		if (d.getName() == "AirplaneStop") {
			this->airplaneStop = {
				sf::Vector3f(
					std::stof(d.getValue("startX")),
					std::stof(d.getValue("startY")),
					std::stof(d.getValue("startZ"))
				),
				sf::Vector3f(
					std::stof(d.getValue("endX")),
					std::stof(d.getValue("endY")),
					std::stof(d.getValue("endZ"))
				),
				sf::Vector3f(
					std::stof(d.getValue("depotX")),
					std::stof(d.getValue("depotY")),
					0.0f
				)
			};
		}
		else if (d.getName() == "TrainStop") {
			this->trainStop = {
				sf::Vector3f(
					d.getValuef("posX"),
					d.getValuef("posY"),
					d.getValuef("posZ")
				),
				IsoRotation(d.getValuef("rot"))
			};
		}
		else if (d.getName() == "CarStop") {
			std::optional<IsoRotation> dir;
			if (d.hasValue("dir")) {
				dir = { IsoRotation(d.getValuef("dir")) };
			}
			this->carStop = {
				sf::Vector3f(
					d.getValuef("posX"),
					d.getValuef("posY"),
					d.getValuef("posZ")
				),
				dir
			};
		}
		else {
			throw std::runtime_error("Invalid SaveData name in TransitStop data");
		}
	}
}
