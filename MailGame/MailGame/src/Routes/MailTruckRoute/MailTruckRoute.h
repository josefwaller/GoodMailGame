#pragma once
#include <vector>
#include <SFML/System/Vector2.hpp>
#include <optional>
#include "Constants.h"
#include "Routes/RoutePoint.h"

class Entity;
class SaveData;

// The individual stop of a route
struct MailTruckRouteStop {
	std::optional<sf::Vector2i> target;
	// The points to get to this stop from the previous stop
	std::vector<RoutePoint> points;
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
	id_t id;
	static id_t ROUTE_ID;
	// Constructor
	MailTruckRoute(bool isDeliv, hour_t time) : isDelivering(isDeliv), departTime(time), id(ROUTE_ID++) {};
};

SaveData mailTruckRouteToSaveData(MailTruckRoute route);
SaveData mailTruckRouteStopToSaveData(MailTruckRouteStop stop);
MailTruckRoute saveDataToMailTruckRoute(SaveData data);
MailTruckRouteStop saveDataToMailTruckRouteStop(SaveData data);
