#pragma once
#include <vector>
#include <SFML/System/Vector2.hpp>
#include <optional>

class Entity;
class SaveData;

// The individual stop of a route
struct MailTruckRouteStop {
	std::optional<sf::Vector2i> target;
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

SaveData mailTruckRouteToSaveData(MailTruckRoute route);
