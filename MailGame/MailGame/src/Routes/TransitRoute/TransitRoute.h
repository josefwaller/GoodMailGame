#pragma once
#include <memory>
#include <vector>
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
	// List of points to get to this stop from the previous stop
	std::vector<RoutePoint> points;
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
	TransitRoute(hour_t time, VehicleModel m) : departureTime(time), id(ROUTE_ID++), model(m) {}
	// The VehicleModel to use along the route
	VehicleModel model;
};

SaveData transitRouteToSaveData(TransitRoute route);
SaveData transitRouteStopToSaveData(TransitRouteStop stop);
TransitRoute saveDataToTransitRoute(Game* g, SaveData d);
TransitRouteStop saveDataToTransitRouteStop(Game* g, SaveData d);
