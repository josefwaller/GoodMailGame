#pragma once
#include "Component/Component.h"
#include <SFML/System/Vector3.hpp>
#include "System/IsoRotation/IsoRotation.h"
#include <optional>

/*
 * Class for holding the details of a transit stop, that is, a place where a train, plane, or truck can stop
 */
class TransitStop : public Component {
public:
	struct CarStop {
		// The tile the car has to get to
		sf::Vector3f tile;
		// The direction the car has to approach the tile
		// Sometimes doesn't matter (i.e. mailboxes)
		std::optional<IsoRotation> dir;
	};
	struct TrainStop {
		// The tile to get to
		sf::Vector3f tile;
		// The direction to enter the tile at
		IsoRotation rot;
	};
	struct AirplaneStop {
		// The tile at the beginning of the runway, i.e. where the plane should land
		sf::Vector3f begin;
		// The tile at the end of the runway, i.e. where the plane should take off from
		sf::Vector3f end;
	};
	enum TransitType {
		Car,
		Train,
		Airplane
	};
	TransitStop(
		std::optional<CarStop> carStop = {},
		std::optional<TrainStop> trainStop = {},
		std::optional<AirplaneStop> airplaneStop = {}
	);
	// Get the location at which the vehicle should stop
	// Different for each type of vehicle
	CarStop getCarStop();
	TrainStop getTrainStop();
	AirplaneStop getAirplaneStop();

	std::optional<SaveData> getSaveData() override;
	void fromSaveData(SaveData data) override;
private:
	std::optional<CarStop> carStop;
	std::optional<TrainStop> trainStop;
	std::optional<AirplaneStop> airplaneStop;
};
