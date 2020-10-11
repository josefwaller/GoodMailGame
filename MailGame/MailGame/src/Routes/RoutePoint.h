#pragma once
#include <SFML/System/Vector3.hpp>
#include "Constants.h"

/*
 * A single point on a route
 * Used by depots to determine where vehicles will go, and by vehicles to determine where to go
 */
struct RoutePoint {
	// The position of the point
	sf::Vector3f pos;
	// The expected arrival time of the point
	gtime_t expectedTime;
	RoutePoint(sf::Vector3f pos, gtime_t time) : pos(pos), expectedTime(time) {};
};
