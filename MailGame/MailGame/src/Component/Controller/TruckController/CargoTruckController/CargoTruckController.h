#pragma once
#include "Component/Controller/TruckController/TruckController.h"
#include "Routes/CargoTruckRoute/CargoTruckRoute.h"
#include <memory>

class Entity;

class CargoTruckController : public TruckController {
public:
	CargoTruckController(CargoTruckRoute route, std::weak_ptr<Entity> office);
	virtual void onArriveAtDest() override;
	virtual float getSpeed() override;
};
