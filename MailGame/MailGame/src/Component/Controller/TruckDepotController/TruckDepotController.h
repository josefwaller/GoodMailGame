#pragma once
#include "Component/Controller/Controller.h"
#include "Routes/CargoTruckRoute/CargoTruckRoute.h"

class TruckDepotController : public Controller {
public:
	virtual void update(float delta) override;
	virtual void onHourChange(size_t newHour) override;
private:
	// The routes assigned to the depot
	std::vector<CargoTruckRoute> routes;
};
