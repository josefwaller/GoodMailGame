#pragma once
#include "Component/Component.h"
#include <SFML/System/Vector3.hpp>
#include "System/IsoRotation/IsoRotation.h"
#include "System/SpeedPoint/SpeedPoint.h"
#include "System/TransitType/TransitType.h"
#include <optional>
#include <vector>

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
		// The depot, i.e. where the airport loads/unloads letters
		sf::Vector3f depot;
		// Whether the runway is in use
		bool isLocked;
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
	// Get a lock on the type of transit
	bool getRunwayLock();
	void releaseRunwayLock();
	// Get the position to travel to for a given entity
	// Split into arriving and departing
	static std::vector<SpeedPoint> getArrivingTransitPath(std::shared_ptr<Entity> e, TransitType type);
	static std::vector<SpeedPoint> getDepartingTransitPath(std::shared_ptr<Entity> e, TransitType type);

	std::optional<SaveData> getSaveData() override;
	void fromSaveData(SaveData data) override;
private:
	std::optional<CarStop> carStop;
	std::optional<TrainStop> trainStop;
	std::optional<AirplaneStop> airplaneStop;
};
