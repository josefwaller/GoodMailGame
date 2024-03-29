#pragma once
#include "Component/Component.h"
#include "Routes/Route.h"
#include <vector>
#include "Component/Controller/VehicleController/VehicleController.h"

/*
 * A component that controls how a vehicle behaves in certain situations
 * Must be used in combination with a VehicleController
 */
class Ai : public Component {
public:
	virtual std::vector<VehicleControllerStop> getStops() = 0;
	virtual void renderUi() = 0;
	virtual void onArriveAtTile(sf::Vector2i tile);
	virtual void onArriveAtStop(size_t stopIndex);
	virtual void onArriveAtDest();
protected:
	// Get the maximum capacity for the route
	unsigned long long getCapacity(Route route);
};
