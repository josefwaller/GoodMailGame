#include "Component/Controller/VehicleController/VehicleController.h"

class PlaneController : public VehicleController {
public:
	enum class State {
		ArrivingAtRunway,
		TaxiingToDest,
		InDepot,
		TaxiingToRunway,
		Departing,
		InTransit,
		WaitingForArrivalLock,
	};
	PlaneController(gtime_t departTime, VehicleModel model);
protected:
	virtual void init() override;
	virtual void update(float delta) override;
	virtual void onArriveAtDest(gtime_t arriveTime) override;

	virtual std::optional<SaveData> getSaveData() override;
	virtual void fromSaveData(SaveData data) override;
private:
	// Utility struct
	struct Runway {
		// The start of the runway, or the part that is connected to the taxiway
		sf::Vector2i start;
		// The end o the runway, or where the airplane should take off into the air
		sf::Vector2i end;
		Runway(sf::Vector2i start, sf::Vector2i end);
	};
	std::vector<VehicleControllerStop> stops;
	size_t stopIndex;
	State state;
	// The runway the plane currently has a lock on for taking off/landing
	Runway runway;
	// The tiles this plane has locked
	std::vector<sf::Vector2i> lockedTiles;
	// Get the path of a loop above an airport while waiting for the runway to be free
	std::vector<SpeedPoint> getLoopPath(RoutePoint start);
	// Get all the runways connected to an entity given
	std::vector<Runway> getAllRunwaysForEntity(std::shared_ptr<Entity> e);
	// Get the taxiing path to a runway
	std::vector<sf::Vector3f> getTaxiPathToRunway(sf::Vector2i from, Runway to);
	// Get the taxiing path to an entity
	std::vector<sf::Vector3f> getTaxiPathToEntity(sf::Vector2i from, std::shared_ptr<Entity> to);
	// Get the taxiing path from a starting point to any one of the end points
	// Will choose whichever one is closest
	std::vector<sf::Vector3f> getTaxiPath(sf::Vector2i from, std::vector<sf::Vector2i> to);
	// Get the points to arrive at a runway
	std::vector<SpeedPoint> getRunwayArrivePoints(Runway r);
	// Get a lock on the runway if possible
	// Return whether the plane was able to get a lock
	bool tryGetLockOnRunway(Runway r);
	// Release the locked tiles
	void releaseLocks();
	// Get the depart points for a given runway
	std::vector<SpeedPoint> getRunwayDepartPoints(Runway r);
	// This method is probably temporary, but maybe not
	std::vector<SpeedPoint> setupTaxiPath(std::vector<sf::Vector3f> taxiPath);
};