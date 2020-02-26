#pragma once
#include <SFML/System/Vector2.hpp>
#include <vector>
#include "Component/Component.h"

/*
 * A component that finds a path between two points
 * Used for vehicle transportation, can be implemented differently for different vehicles
 */
class Pathfinder: public Component {
public:
	virtual std::vector<sf::Vector2f> findPathBetweenPoints(sf::Vector2f from, sf::Vector2f to) = 0;
};
