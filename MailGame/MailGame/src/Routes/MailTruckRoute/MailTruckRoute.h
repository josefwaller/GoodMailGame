#pragma once
#include <vector>

class Entity;

// The individual stop of a route
struct MailTruckRouteStop {
	sf::Vector2i stopCoords;
};

// A route, with a list of stops and a depart time
struct MailTruckRoute {
	// Whether the route is delivering or picking up letters
	bool isDelivering;
	// The stops in the route
	std::vector<MailTruckRouteStop> stops;
	// The time of departure
	int departTime;
	// Unique Id
	size_t id;
	static size_t ROUTE_ID;
	// Constructor
	MailTruckRoute(bool isDeliv, int time) : isDelivering(isDeliv), departTime(time), id(ROUTE_ID++) {};
};
