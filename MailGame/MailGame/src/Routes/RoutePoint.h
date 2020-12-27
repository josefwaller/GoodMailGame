#pragma once
#include <SFML/System/Vector3.hpp>
#include "Constants.h"
#include "System/SaveData/SaveData.h"

/*
 * A single point on a route
 * Used by depots to determine where vehicles will go, and by vehicles to determine where to go
 */
struct RoutePoint {
	// The position of the point
	sf::Vector3f pos;
	// The expected arrival time of the point
	gtime_t expectedTime;
	// The distance this point is along the route
	// Todo: Make sure this is always set
	float distance;
	RoutePoint(sf::Vector3f pos, gtime_t time, float distance = 0.0f) : pos(pos), expectedTime(time), distance(distance) {};
};

SaveData routePointToSaveData(RoutePoint p);
RoutePoint saveDataToRoutePoint(SaveData d);
