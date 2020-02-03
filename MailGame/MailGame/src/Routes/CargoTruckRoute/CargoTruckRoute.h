#pragma once
#include <memory>
#include <vector>

class Entity;

// An individual stop along the route
struct CargoTruckStop {
	std::weak_ptr<Entity> target;
};

// The route, with all the stops
struct CargoTruckRoute {
	std::vector<CargoTruckStop> stops;
	// When to leave the depot
	int departureTime;
	// The id
	unsigned long long id;
	static unsigned long long ROUTE_ID;
	// Constructor
	CargoTruckRoute(int time) : departureTime(time), id(ROUTE_ID++) {}
};