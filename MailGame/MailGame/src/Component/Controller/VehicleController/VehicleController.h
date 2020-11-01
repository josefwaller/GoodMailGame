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
	// The path the vehicle must take to arrive at this stop
	std::vector<sf::Vector3f> arrivingPath;
	std::vector<sf::Vector3f> departingPath;
	// The time the vehicle expects to arrive at this stop
	// Set when the vehicle gets to the previous stop
	gtime_t expectedTime;
	// The time to wait at this stop
	gtime_t waitTime;
	VehicleControllerStop(std::vector<sf::Vector3f> departingPath, std::vector<sf::Vector3f> arrivingPath, gtime_t waitTime = 0)
		: arrivingPath(arrivingPath), departingPath(departingPath), waitTime(waitTime), expectedTime(0) {}
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
	// Get the path, as route points, between two stops
	// Includes the departing path and the arriving path
	std::vector<RoutePoint> getPathBetweenStops(VehicleControllerStop from, VehicleControllerStop to);
	// Go to the next stop along the route
	void goToNextStop();
	// Get the distance to travel between two points, via the path returned by the pathfinder
	float getPathDistance(sf::Vector3f from, sf::Vector3f to);
	VehicleModel model;
};

