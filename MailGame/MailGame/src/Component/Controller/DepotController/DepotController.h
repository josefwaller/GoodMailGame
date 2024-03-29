#pragma once
#include "Component/Controller/Controller.h"
#include "Routes/TransitRoute/TransitRoute.h"
#include "Component/TransitStop/TransitStop.h"
#include "VehicleModel/VehicleModel.h"
#include "System/TransitType/TransitType.h"
#include <map>
#include <vector>

/*
 * Superclass for entity controllers which spawn trucks/trains/boats/planes/etc to bring mail between places
 * Has a UI that controls the routes and holds the routes assigned to this depot
 * Has abstract methods which must be filled out by the subclass for the specific type of depot
 */
class DepotController : public Controller {
public:
	DepotController(TransitType type, std::vector<VehicleModel> availableModels);
	virtual void update(float delta) override;
	virtual void onHourChange(hour_t newHour) override;
	virtual void renderUi() override;
	virtual money_t getCost() override;
	// Spawn a vehicle for the given route
	virtual void spawnVehicleForRoute(TransitRoute route) = 0;
	// Get the name for the UI window
	virtual const char* getUiName() = 0;

	/**** Methods used by UI to modify routes ****/
	// Add a new route
	void addRoute(TransitRoute route);
	// Delete a route by id
	void deleteRoute(size_t routeId);
	// Set a route's depart time
	void setRouteDepartTime(size_t routeId, int depTime);
	// Set the model for a route
	void setRouteModel(size_t routeId, VehicleModel model);
	// Set the cargo car model for a route
	void setRouteCargoCarModel(size_t routeId, std::optional<CargoCarModel> model);
	// Set the number of cargo cars for a route
	void setRouteNumberCargoCars(size_t routeId, unsigned int num);
	// Add a stop to the route by id
	void addStop(TransitRouteStop stop, size_t routeId);
	// Delete a stop by index from the route by id
	void deleteStop(size_t stopIndex, size_t routeId);
	// Set a stop's target
	void setStopTarget(size_t stopIndex, size_t routeId, std::weak_ptr<Entity> target);
	// Set a code to be picked up for a stop
	void setStopPickUp(size_t stopIndex, size_t routeIndex, long long code);
	// Remove a code to be picked up for a stop
	void removeStopPickUp(size_t stopIndex, size_t routeIndex, long long code);
	// Set a code to be dropped off for a stop
	void setStopDropOff(size_t stopIndex, size_t routeIndex, long long code);
	// Remove a code to be picked up for a stop
	void removeStopDropOff(size_t stopIndex, size_t routeIndex, long long code);

	// Recalculate the length of the given route
	// Should be implemented by the children classes as different types of vehicles follow different routes
	virtual void resetRouteLength(size_t routeId) = 0;

	virtual std::optional<SaveData> getSaveData() override;
	virtual void fromSaveData(SaveData data) override;

protected:
	TransitRoute prepareRouteForVehicle(TransitRoute route);
	// The routes assigned to the depot, keyed by id
	std::map<size_t, TransitRoute> routes;
private:
	// The routes to delete, done after drawing UI in order to prevent conflicts
	std::vector<TransitRoute> toDelete;
	// The models available
	std::vector<VehicleModel> availableModels;
	// The type of transit
	TransitType type;
	// This is basically a debug feature, but toggle to show the routes of this station on the UI
	bool showRoutes;
	// Last spawn time
	gtime_t lastSpawnTime;
};
