#pragma once
#include "Component/Controller/Controller.h"
#include <vector>
#include <SFML/System/Vector2.hpp>

class GameMap;

class VehicleController: public Controller {
public:
	virtual void update(float delta) override;
protected:
	// Set the stops
	void setStops(std::vector<sf::Vector2f> stops);
	// Methods to be overridden by subclass
	// When the truck arrives at a tile
	virtual void onArriveAtTile(sf::Vector2f point);
	// When the truck arrives at a stop
	virtual void onArriveAtStop(size_t stopIndex);
	// When the truck arrives at the last stop, i.e. the route is complete
	virtual void onArriveAtDest();
	// Get the speed the truck should move at
	virtual float getSpeed() = 0;
private:
	// The points the truck is currently going through on its route
	std::vector<sf::Vector2f> points;
	size_t pointIndex;
	// The stops and the index of the current stop
	std::vector<sf::Vector2f> stops;
	size_t stopIndex;
	// Set points as the path from its current position to the point given
	void pathfindToPoint(sf::Vector2f point);
	// Go to the next stop along the route
	void goToNextStop();
};
