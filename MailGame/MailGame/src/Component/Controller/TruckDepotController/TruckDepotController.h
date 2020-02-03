#pragma once
#include "Component/Controller/Controller.h"
#include "Routes/CargoTruckRoute/CargoTruckRoute.h"
#include <map>

class TruckDepotController : public Controller {
public:
	virtual void update(float delta) override;
	virtual void onHourChange(size_t newHour) override;

	// Add a new route
	void addRoute(CargoTruckRoute route);
	// Delete a route by id
	void deleteRoute(size_t routeId);
	// Add a stop to the route by id
	void addStop(CargoTruckStop stop, size_t routeId);
	// Delete a stop by index from the route by id
	void deleteStop(size_t stopIndex, size_t routeId);
	// Set a stop's target
	void setStopTarget(size_t stopIndex, size_t routeId, std::weak_ptr<Entity> target);
private:
	// The routes assigned to the depot, keyed by id
	std::map<size_t, CargoTruckRoute> routes;
	// The routes to delete, done after drawing UI in order to prevent conflicts
	std::vector<CargoTruckRoute> toDelete;
};
