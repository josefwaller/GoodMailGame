#pragma once
#include "Component/Controller/TruckController/TruckController.h"
#include "Routes/TransitRoute/TransitRoute.h"
#include <memory>

class Entity;

class CargoTruckController : public TruckController {
public:
	CargoTruckController(TransitRoute route, std::weak_ptr<Entity> office);
	// Overridden methods, see TruckController
	virtual void onArriveAtDest() override;
	virtual void onArriveAtStop(size_t stopIndex) override;
	virtual float getSpeed() override;
private:
	// The depot that the truck started at, and will return to
	std::weak_ptr<Entity> depot;
	// The route the truck is following
	TransitRoute route;
};
