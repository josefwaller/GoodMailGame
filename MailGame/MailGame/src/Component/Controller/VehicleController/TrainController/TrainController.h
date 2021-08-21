#pragma once
#include "Component/Controller/VehicleController/VehicleController.h"
#include "GameMap/Tile/Railway/Railway.h"
#include "Component/Pathfinder/RailsPathfinder/RailsPathfinder.h"
#include <queue>

class TrainController : public VehicleController {
public:
	static enum class State {
		InTransit,
		ArrivingAtStation,
	};
	TrainController(gtime_t departTime, VehicleModel model, std::vector<std::weak_ptr<Entity>> trainCars = {});
	virtual void init() override;
	virtual void update(float delta) override;
	virtual void onDelete() override;
protected:
	std::vector<SpeedPoint> getPointsForSegment(RailsPathfinder::Segment s);
	virtual void onArriveAtDest(gtime_t arriveTime) override;

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
	// The tiles the train has a lock on
	std::queue<std::pair<sf::Vector2i, Railway>> lockedRailways;
	// Get the length of the train
	float getTrainLength();
	// Get the path to dock at this entity
	// Will eventually return several paths, and the train must choose which one
	std::vector<std::pair<sf::Vector2i, Railway>> getDockPath(std::shared_ptr<Entity> e);
	// Get the tile the train can go to, used for running pathfinding again when the train is blocked
	sf::Vector2i getDest();
};
