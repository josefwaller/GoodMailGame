#pragma once
#include <SFML/System/Vector3.hpp>
#include <vector>
#include "Component/Component.h"
#include "Routes/RoutePoint.h"

/*
 * A component that finds a path between two points
 * Used for vehicle transportation, can be implemented differently for different vehicles
 */
class Pathfinder: public Component {
public:
	virtual std::vector<RoutePoint> findPathBetweenPoints(
		sf::Vector3f from,
		sf::Vector3f to,
		gtime_t departTime,
		float speed) = 0;
protected:
	gtime_t getTimeBetween(sf::Vector3f from, sf::Vector3f to, float speed);
};
