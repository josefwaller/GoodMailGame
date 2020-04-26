#pragma once
#include <SFML/System/Vector3.hpp>
#include <vector>
#include "Component/Component.h"

/*
 * A component that finds a path between two points
 * Used for vehicle transportation, can be implemented differently for different vehicles
 */
class Pathfinder: public Component {
public:
	virtual std::vector<sf::Vector3f> findPathBetweenPoints(sf::Vector3f from, sf::Vector3f to) = 0;
};
