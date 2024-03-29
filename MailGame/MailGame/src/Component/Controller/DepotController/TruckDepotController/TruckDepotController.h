#pragma once
#include "Component/Controller/DepotController/DepotController.h"
#include <map>

class TruckDepotController : public DepotController {
public:
	TruckDepotController();
	virtual void spawnVehicleForRoute(TransitRoute route) override;
	const char* getUiName() override;
	virtual void resetRouteLength(size_t routeId) override;
};
