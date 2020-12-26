#pragma once
#include <memory>
#include <vector>
#include <optional>
#include "Constants.h"
#include "Routes/RoutePoint.h"
#include "VehicleModel/VehicleModel.h"

class Entity;
class SaveData;
class Game;

/*
 * An individual stop along a TransitRoute
 */
struct TransitRouteStop {
	// The entity the stop is at
	std::weak_ptr<Entity> target;
	// A list of the postal codes for the vehicle to drop off/pick up
	std::vector<long long> toDropOff;
	std::vector<long long> toPickUp;
	// Id of the stop (Todo: Remove this when not needed by ImGui
	id_t id;
	static id_t STOP_ID;
	TransitRouteStop() : id(STOP_ID++) {}
};

/*
 * A route that goes between different buildings and picks up/drops off mail depending on the postal codes
 * Used by trucks, trains, planes, and boats
 */
struct TransitRoute {
	// The stops along the route
	std::vector<TransitRouteStop> stops;
	// When to leave the depot
	hour_t departureTime;
	// The id
	id_t id;
	static id_t ROUTE_ID;
	// Constructor
	TransitRoute(hour_t time, VehicleModel m, std::optional<VehicleModel> s = {}) : departureTime(time), id(ROUTE_ID++), model(m), secondaryModel(s) {}
	TransitRoute(hour_t time, VehicleModel m, VehicleModel s) : TransitRoute(time, m, std::optional<VehicleModel>(s)) {}
	// The VehicleModel to use along the route
	VehicleModel model;
	// optional second vehicle model, for things like the train's cars
	std::optional<VehicleModel> secondaryModel;
};

SaveData transitRouteToSaveData(TransitRoute route);
SaveData transitRouteStopToSaveData(TransitRouteStop stop);
TransitRoute saveDataToTransitRoute(Game* g, SaveData d);
TransitRouteStop saveDataToTransitRouteStop(Game* g, SaveData d);
