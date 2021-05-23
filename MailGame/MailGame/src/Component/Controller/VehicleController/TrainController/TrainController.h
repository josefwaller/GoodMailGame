#pragma once
#include "Component/Controller/VehicleController/VehicleController.h"
#include "GameMap/Tile/Railway/Railway.h"
#include <queue>

class TrainController : public VehicleController {
public:
	static enum class State {
		InTransit,
		ArriveAtStation
	};
	TrainController(gtime_t departTime, VehicleModel model, std::vector<std::weak_ptr<Entity>> trainCars = {});
	virtual void init() override;
	virtual void update(float delta) override;
	virtual void onDelete() override;
protected:
	virtual void onArriveAtPoint(size_t pointIndex, gtime_t arriveTime) override;
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
	std::vector<std::pair<sf::Vector2i, Railway>> path;
	// The tiles the train has a lock on
	std::queue<std::pair<sf::Vector2i, Railway>> lockedRailways;
	// Get the length of the train
	float getTrainLength();
	// Get the path to dock at this entity
	// Will eventually return several paths, and the train must choose which one
	std::vector<std::pair<sf::Vector2i, Railway>> getDockPath(std::shared_ptr<Entity> e);
	// Reset the path the train is taking
	void resetPath(sf::Vector2i fromTile, sf::Vector2i toTile, IsoRotation startingRotation, gtime_t departTime);
};
