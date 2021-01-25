#pragma once
#include "Component/Controller/Controller.h"
#include "Component/TransitStop/TransitStop.h"
#include "Routes/RoutePoint.h"
#include "VehicleModel/VehicleModel.h"
#include "System/SpeedPoint/SpeedPoint.h"
#include <vector>
#include <SFML/System/Vector3.hpp>
#include <SFML/System/Vector2.hpp>

class GameMap;

// Data structure for each stop
struct VehicleControllerStop {
	// The path the vehicle must take to arrive at this stop
	std::vector<SpeedPoint> arrivingPath;
	std::vector<SpeedPoint> departingPath;
	// The time the vehicle expects to arrive at this stop
	// Set when the vehicle gets to the previous stop
	gtime_t expectedTime;
	// The distance this stop is on the route
	// i.e. how far between this stop and the first stop
	float distance;
	// The time to wait at this stop
	gtime_t waitTime;
	VehicleControllerStop(std::vector<SpeedPoint> departingPath, std::vector<SpeedPoint> arrivingPath, gtime_t waitTime = 0)
		: arrivingPath(arrivingPath), departingPath(departingPath), waitTime(waitTime), expectedTime(0), distance(0) {}
};

class VehicleController : public Controller {
public:
	VehicleController(gtime_t departTime, VehicleModel model, std::vector<std::weak_ptr<Entity>> cargoCars = {});
	virtual void update(float delta) override;
	virtual void onDelete() override;
protected:
	// Callback every time the vehicle reaches a point
	virtual void onArriveAtPoint(size_t pointIndex, gtime_t arriveTime);
	// Callback when the vehicle finishes its current path
	virtual void onArriveAtDest(gtime_t arriveTime);
	// Set the points to traverse between
	void setPoints(std::vector<RoutePoint> points);
	// Delete the cars
	// Called when the vehicle has reached its final destination
	void deleteCars();
	// Sets the cargo cars
	void setCargoCars(std::vector<std::weak_ptr<Entity>> cc);
	// Sets up the stops on load
	// Mainly sets the expected time so that things will run smoothly
	// Make sure to call this after setting the stops
	virtual void fromSaveData(SaveData data) override;
	// Get the save data
	// For this class, it should be called by a child class and combined with another save data
	std::optional<SaveData> getSaveData() override;
	// The points the truck is currently going through on its route
	std::vector<RoutePoint> points;
	size_t pointIndex;
	// The model that this vehicle is
	VehicleModel model;
	// The cars
	std::vector<std::weak_ptr<Entity>> cargoCars;
	// All the points the vehicle has gone to, in order
	// Used for setting up the cargo cars
	std::vector<RoutePoint> traversedPoints;

	gtime_t departTime;
private:
	// Get the distance to travel between two points, via the path returned by the pathfinder
	float getPathDistance(sf::Vector3f from, sf::Vector3f to);
	// Get the position and rotation on the path after travelling the distance given
	std::pair<sf::Vector3f, IsoRotation> getPosAndRotAtDistance(float distance);
};
