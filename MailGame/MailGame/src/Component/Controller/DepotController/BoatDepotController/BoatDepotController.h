#include "Component/Controller/DepotController/DepotController.h"

class BoatDepotController : public DepotController {
public:
	BoatDepotController();
	virtual void spawnVehicleForRoute(TransitRoute route) override;
	virtual const char* getUiName() override;
};