#pragma once
#include <memory>
#include <vector>

class Entity;
class SaveData;

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
	unsigned long long id;
	static unsigned long long STOP_ID;
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
	int departureTime;
	// The id
	unsigned long long id;
	static unsigned long long ROUTE_ID;
	// Constructor
	TransitRoute(int time) : departureTime(time), id(ROUTE_ID++) {}
};

SaveData transitRouteToSaveData(TransitRoute route);
SaveData transitRouteStopToSaveData(TransitRouteStop stop);
