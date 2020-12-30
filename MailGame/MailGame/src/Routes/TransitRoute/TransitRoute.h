#pragma once
#include <memory>
#include <vector>
#include <optional>
#include "Constants.h"
#include "Routes/Route.h"
#include "VehicleModel/CargoCarModel/CargoCarModel.h"
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
};

/*
 * A route that goes between different buildings and picks up/drops off mail depending on the postal codes
 * Used by trucks, trains, planes, and boats
 */
struct TransitRoute : public Route {
	// The stops along the route
	std::vector<TransitRouteStop> stops;
	// Constructor
	// This one will usually just leave out CargoCarModel
	TransitRoute(hour_t time, VehicleModel m, std::optional<CargoCarModel> s = {}, unsigned int num = 0) : Route(time, m, s, num) {}
	TransitRoute(hour_t time, VehicleModel m, CargoCarModel s, unsigned int num) : Route(time, m, std::optional<CargoCarModel>(s), num) {}
};

SaveData transitRouteToSaveData(TransitRoute route);
SaveData transitRouteStopToSaveData(TransitRouteStop stop);
TransitRoute saveDataToTransitRoute(Game* g, SaveData d);
TransitRouteStop saveDataToTransitRouteStop(Game* g, SaveData d);
