#pragma once
#include "Component/Controller/DepotController/DepotController.h"
#include <map>

class TruckDepotController : public DepotController {
public:
	virtual void spawnVehicleForRoute(TransitRoute route) override;
	const char* getUiName() override;
};
