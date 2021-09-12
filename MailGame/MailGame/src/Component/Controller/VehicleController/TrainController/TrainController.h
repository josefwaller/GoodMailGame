#pragma once
#include "Component/Controller/VehicleController/VehicleController.h"
#include "GameMap/Tile/Railway/Railway.h"
#include "Component/Pathfinder/RailsPathfinder/RailsPathfinder.h"
#include "./RailLock.h"
#include <deque>

class TrainController : public VehicleController {
public:
	static enum class State {
		InTransit,
		Blocked,
		ArrivingAtStation,
	};
	TrainController(gtime_t departTime, VehicleModel model, std::vector<std::weak_ptr<Entity>> trainCars = {});
	virtual void init() override;
	virtual void update(float delta) override;
	virtual void onDelete() override;
protected:
	std::vector<SpeedPoint> getPointsForSegment(RailsPathfinder::Segment s, std::optional<int> endingSpeed = {});
	bool tryGetLockForSegment(RailsPathfinder::Segment segment);
	virtual void onArriveAtDest(gtime_t arriveTime) override;
	virtual void onArriveAtPoint(gtime_t arriveTime, size_t pointIndex) override;

	virtual std::optional<SaveData> getSaveData() override;
	virtual void fromSaveData(SaveData data) override;
private:
	// The stops the train is going over
	std::vector<VehicleControllerStop> stops;
	// The current index of the stop it's going to
	size_t stopIndex;
	// The current state of the train
	State state;
	// The path of railways the train is taking
	std::vector<RailsPathfinder::Segment> path;
	// The current segment it's going through
	size_t pathIndex;
	// The destination, or tile the train is currently heading to
	sf::Vector2i dest;
	// The tiles/tunnels/bridges that the train has a lock on
	std::vector<RailLock> railLocks;
	// Get the length of the train
	float getTrainLength();
	// Try and get unblocked
	void tryGetUnblocked(sf::Vector2i from, std::vector<sf::Vector2i> to, IsoRotation rot, float startingSpeed);
	// Get all the tiles that are valid "docking" points
	// i.e. get all the places the train can go to dock with this entity
	// Usually just get the tiles at the end of the connected train station
	std::vector<sf::Vector2i> getDockTiles(std::shared_ptr<Entity> e);
	// Get the tile the train can go to, used for running pathfinding again when the train is blocked
	sf::Vector2i getDest();
};
