#pragma once
#include "Component/Controller/DepotController/DepotController.h"

class PlaneDepotController : public DepotController {
public:
	PlaneDepotController();
	virtual void spawnVehicleForRoute(TransitRoute route) override;
	virtual const char* getUiName() override;
};
