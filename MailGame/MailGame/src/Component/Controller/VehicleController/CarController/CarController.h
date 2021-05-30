#pragma once
#include "Component/Controller/VehicleController/VehicleController.h"

class CarController : public VehicleController {
public:
	CarController(gtime_t departTime, VehicleModel model, std::vector<std::weak_ptr<Entity>> cargoCars = {});
	virtual void init() override;
	virtual void update(float delta) override;
protected:
	virtual void onArriveAtPoint(size_t pointIndex, gtime_t arriveTime) override;
	virtual void onArriveAtDest(gtime_t arriveTime) override;
	virtual std::optional<SaveData> getSaveData() override;
	virtual void fromSaveData(SaveData) override;
private:
	std::vector<VehicleControllerStop> stops;
	size_t stopIndex;
	// Get the path between two stops
	std::vector<SpeedPoint> getPathBetweenStops(VehicleControllerStop from, VehicleControllerStop to);
	// Get the dock coordinates for a given entity
	std::vector<sf::Vector2i> getDockTiles(std::shared_ptr<Entity> e);
	// The path the car is taking
	std::vector<sf::Vector2i> path;
	// Set the points for VehicleController to use
	void resetPath();
};
