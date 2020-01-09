#pragma once
#include <vector>

class Entity;

// The individual stop of a route
struct MailTruckRouteStop {

};

// A route, with a list of stops and a depart time
struct MailTruckRoute {
	// Whether the route is delivering or picking up letters
	bool isDelivering;
	// The stops in the route
	std::vector<MailTruckRouteStop> stop;
	// The time of departure
	int departTime;
	// Constructor
	MailTruckRoute(bool isDeliv, int time) : isDelivering(isDeliv), departTime(time) {};
};
