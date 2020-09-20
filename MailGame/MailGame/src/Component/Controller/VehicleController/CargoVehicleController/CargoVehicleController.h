#pragma once
#include "Component/Controller/VehicleController/VehicleController.h"
#include "Component/TransitStop/TransitStop.h"
#include "Routes/TransitRoute/TransitRoute.h"
#include <memory>

class Entity;

class CargoVehicleController : public VehicleController {
public:
	CargoVehicleController(TransitRoute route, std::weak_ptr<Entity> office, TransitStop::TransitType type, gtime_t departTime);
	// Set stops in vehicle controller based on route
	void setRouteStops();
	// Overridden methods, see TruckController
	virtual void onArriveAtDest() override;
	virtual void onArriveAtStop(size_t stopIndex) override;
	virtual float getSpeed() override;
	virtual std::optional<SaveData> getSaveData() override;
	virtual void fromSaveData(SaveData data) override;
private:
	// The depot that the truck started at, and will return to
	std::weak_ptr<Entity> depot;
	// The route the truck is following
	TransitRoute route;
	// The type of stop this vehicle will access
	TransitStop::TransitType type;
};
