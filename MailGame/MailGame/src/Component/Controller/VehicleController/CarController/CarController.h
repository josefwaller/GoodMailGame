#pragma once
#include "Component/Controller/VehicleController/VehicleController.h"

class CarController : public VehicleController {
public:
	CarController(gtime_t departTime, VehicleModel model, std::vector<std::weak_ptr<Entity>> cargoCars = {});
	void update(float delta);
protected:
	virtual void onArriveAtPoint(size_t pointIndex, gtime_t arriveTime) override;
	virtual void onArriveAtDest(gtime_t arriveTime) override;
private:
	std::vector<VehicleControllerStop> stops;
	size_t stopIndex;

	std::vector<RoutePoint> getPathBetweenStops(VehicleControllerStop from, VehicleControllerStop to, gtime_t t);
};
