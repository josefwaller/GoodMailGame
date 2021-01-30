#pragma once
#include "Component/Controller/VehicleController/VehicleController.h"

class TrainController : public VehicleController {
public:
	static enum class State {
		DepartingStation,
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
	std::vector<VehicleControllerStop> stops;
	size_t stopIndex;
	State state;
	bool isBlocked;
	std::vector<sf::Vector2i> lockedTiles;
};
