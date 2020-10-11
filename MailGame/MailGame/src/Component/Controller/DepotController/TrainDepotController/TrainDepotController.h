#pragma once
#include "Component/Controller/DepotController/DepotController.h"

class TrainDepotController : public DepotController {
public:
	TrainDepotController();
	virtual void spawnVehicleForRoute(TransitRoute r) override;
	virtual const char* getUiName() override;
};
