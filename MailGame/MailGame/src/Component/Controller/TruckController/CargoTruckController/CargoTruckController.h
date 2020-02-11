#pragma once
#include "Component/Controller/TruckController/TruckController.h"
#include "Routes/CargoTruckRoute/CargoTruckRoute.h"
#include <memory>

class Entity;

class CargoTruckController : public TruckController {
public:
	CargoTruckController(CargoTruckRoute route, std::weak_ptr<Entity> office);
	// Overridden methods, see TruckController
	virtual void onArriveAtDest() override;
	virtual void onArriveAtStop(size_t stopIndex) override;
	virtual float getSpeed() override;
private:
	// The depot that the truck started at, and will return to
	std::weak_ptr<Entity> depot;
	// The route the truck is following
	CargoTruckRoute route;
};
