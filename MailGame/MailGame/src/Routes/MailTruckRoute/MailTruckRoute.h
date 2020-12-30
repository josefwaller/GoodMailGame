#pragma once
#include <vector>
#include <SFML/System/Vector2.hpp>
#include <optional>
#include "Constants.h"
#include "Routes/RoutePoint.h"
#include "Routes/Route.h"

class Entity;
class SaveData;

// The individual stop of a route
struct MailTruckRouteStop {
	std::optional<sf::Vector2i> target;
};

// A route, with a list of stops and a depart time
struct MailTruckRoute : Route {
	// Whether the route is delivering or picking up letters
	bool isDelivering;
	// The stops in the route
	std::vector<MailTruckRouteStop> stops;
	// Constructor
	MailTruckRoute(bool isDeliv, hour_t time) : isDelivering(isDeliv), Route(time, VehicleModel::MailTruck) {};
};

SaveData mailTruckRouteToSaveData(MailTruckRoute route);
SaveData mailTruckRouteStopToSaveData(MailTruckRouteStop stop);
MailTruckRoute saveDataToMailTruckRoute(SaveData data);
MailTruckRouteStop saveDataToMailTruckRouteStop(SaveData data);
