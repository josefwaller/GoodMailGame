#pragma once
#include "Component/Controller/Controller.h"
#include <vector>
#include <SFML/System/Vector2.hpp>

class GameMap;

class TruckController : public Controller {
public:
	virtual void update(float delta) override;
	// Get the tiles on the path between two points
	static std::vector<sf::Vector2i> getPathBetween(sf::Vector2i from, sf::Vector2i to, GameMap* gMap);
protected:
	// The points the truck is currently going through on its route
	std::vector<sf::Vector2i> points;
	size_t pointIndex;
	// Set points as the path from its current position to the point given
	void pathfindToPoint(sf::Vector2i point);
	// Methods to be overridden by subclass
	virtual void onArriveAtDest() = 0;
	virtual void onArriveAtTile(sf::Vector2f point) = 0;
	virtual float getSpeed() = 0;

};
