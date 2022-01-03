#pragma once
#include "Component/Controller/DepotController/DepotController.h"

class TrainDepotController : public DepotController {
public:
	TrainDepotController();
	virtual void init();
	virtual void spawnVehicleForRoute(TransitRoute r) override;
	virtual const char* getUiName() override;
	virtual void resetRouteLength(size_t routeId) override;
};
