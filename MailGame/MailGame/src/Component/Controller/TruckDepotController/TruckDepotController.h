#pragma once
#include "Component/Controller/Controller.h"
#include "Routes/CargoTruckRoute/CargoTruckRoute.h"

class TruckDepotController : public Controller {
public:
	virtual void update(float delta) override;
	virtual void onHourChange(size_t newHour) override;

	// Add a new route
	void addRoute(CargoTruckRoute route);
	// Delete a route by id
	void deleteRoute(size_t routeId);
private:
	// The routes assigned to the depot
	std::vector<CargoTruckRoute> routes;
};
