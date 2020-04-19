#pragma once
#include "Component/Controller/VehicleController/VehicleController.h"
#include "Routes/TransitRoute/TransitRoute.h"
#include <memory>

class Entity;

class CargoVehicleController : public VehicleController {
public:
	CargoVehicleController(TransitRoute route, std::weak_ptr<Entity> office);
	// Overridden methods, see TruckController
	virtual void onArriveAtDest() override;
	virtual void onArriveAtStop(size_t stopIndex) override;
	virtual float getSpeed() override;
	virtual SaveData getSaveData() override;
private:
	// The depot that the truck started at, and will return to
	std::weak_ptr<Entity> depot;
	// The route the truck is following
	TransitRoute route;
};
