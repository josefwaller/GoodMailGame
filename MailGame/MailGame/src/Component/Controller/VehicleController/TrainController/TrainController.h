#pragma once
#include "Component/Controller/VehicleController/VehicleController.h"

class TrainController : public VehicleController {
public:
	static enum class State {
		InTransit,
		ArriveAtStation
	};
	TrainController(gtime_t departTime, VehicleModel model, std::vector<std::weak_ptr<Entity>> trainCars = {});
	virtual void update(float delta) override;
	virtual void onDelete() override;
protected:
	virtual void onArriveAtPoint(size_t pointIndex, gtime_t arriveTime) override;
	virtual void onArriveAtDest(gtime_t arriveTime) override;
private:
	// The stops the train is going over
	std::vector<VehicleControllerStop> stops;
	// The current index of the stop it's going to
	size_t stopIndex;
	// The current state of the train
	State state;
	// Whether the train is blocked or not
	bool isBlocked;
	// The current tiles that the train has a lock on
	// Stored to remove the possibility of an orphan locked tile
	std::vector<sf::Vector2i> lockedTiles;
	// Get the path to dock at this entity
	// Will eventually return several paths, and the train must choose which one
	std::vector<SpeedPoint> getDockPath(std::shared_ptr<Entity> e);
};
