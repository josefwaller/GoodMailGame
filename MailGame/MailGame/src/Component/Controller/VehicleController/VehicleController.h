#pragma once
#include "Component/Controller/Controller.h"
#include "Component/TransitStop/TransitStop.h"
#include "Routes/RoutePoint.h"
#include "VehicleModel/VehicleModel.h"
#include <vector>
#include <SFML/System/Vector3.hpp>
#include <SFML/System/Vector2.hpp>

class GameMap;

// Data structure for each stop
struct VehicleControllerStop {
	// The position the vehicle will go to
	sf::Vector3f pos;
	// The time the vehicle expects to arrive at this stop
	// Set when the vehicle gets to the previous stop
	gtime_t expectedTime;
	std::vector<RoutePoint> points;
	VehicleControllerStop(sf::Vector3f pos, gtime_t time)
		: pos(pos), expectedTime(time) {}
};

class VehicleController: public Controller {
public:
	VehicleController(gtime_t departTime, VehicleModel model);
	virtual void update(float delta) override;
protected:
	// Set the stops
	void setStops(std::vector<VehicleControllerStop> stops);
	// Methods to be overridden by subclass
	// When the truck arrives at a tile
	virtual void onArriveAtTile(sf::Vector2i point);
	// When the truck arrives at a stop
	virtual void onArriveAtStop(size_t stopIndex);
	// When the truck arrives at the last stop, i.e. the route is complete
	virtual void onArriveAtDest();
	// Get the speed the truck should move at
	virtual float getSpeed();
	// Children need access to this for saving/loading
	size_t stopIndex;
	// The time the vehicle departed the depot
	gtime_t departTime;
	// Sets up the stops on load
	// Mainly sets the expected time so that things will run smoothly
	// Make sure to call this after setting the stops
	virtual void fromSaveData(SaveData data) override;
	// Get eth save data
	// For this class, it should be called by a child class and combined with another save data
	std::optional<SaveData> getSaveData() override;
private:
	// The points the truck is currently going through on its route
	std::vector<RoutePoint> points;
	size_t pointIndex;
	// The stops and the index of the current stop
	std::vector<VehicleControllerStop> stops;
	// Set points as the path from its current position to the point given
	void pathfindToPoint(sf::Vector3f from, sf::Vector3f point);
	// Go to the next stop along the route
	void goToNextStop();
	// Get the distance to travel between two points, via the path returned by the pathfinder
	float getPathDistance(sf::Vector3f from, sf::Vector3f to);
	VehicleModel model;
};

