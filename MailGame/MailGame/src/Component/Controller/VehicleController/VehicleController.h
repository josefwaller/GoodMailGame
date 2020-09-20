#pragma once
#include "Component/Controller/Controller.h"
#include <vector>
#include <SFML/System/Vector3.hpp>
#include <SFML/System/Vector2.hpp>

class GameMap;

// Data structure for each stop
struct VehicleControllerStop {
	sf::Vector3f pos;
	gtime_t expectedTime;
	VehicleControllerStop(sf::Vector3f pos, gtime_t time) : pos(pos), expectedTime(time) {}
};

class VehicleController: public Controller {
public:
	VehicleController(gtime_t departTime);
	virtual void update(float delta) override;
protected:
	// Set the stops
	void setStops(std::vector<sf::Vector3f> stops);
	// Methods to be overridden by subclass
	// When the truck arrives at a tile
	virtual void onArriveAtTile(sf::Vector2i point);
	// When the truck arrives at a stop
	virtual void onArriveAtStop(size_t stopIndex);
	// When the truck arrives at the last stop, i.e. the route is complete
	virtual void onArriveAtDest();
	// Get the speed the truck should move at
	virtual float getSpeed() = 0;
	// Children need access to this for saving/loading
	size_t stopIndex;
private:
	// The points the truck is currently going through on its route
	std::vector<VehicleControllerStop> points;
	size_t pointIndex;
	// The stops and the index of the current stop
	std::vector<VehicleControllerStop> stops;
	// Set points as the path from its current position to the point given
	void pathfindToPoint(sf::Vector3f from, sf::Vector3f point);
	// Go to the next stop along the route
	void goToNextStop();
	// The time the vehicle departed the depot
	gtime_t departTime;
};

