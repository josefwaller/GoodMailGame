#pragma once
#include "Component/Pathfinder/Pathfinder.h"

/*
 * A pathfinder that finds a path between two points by going along roads
 */
class RoadPathfinder : public Pathfinder {
public:
	virtual std::vector<sf::Vector3f> findPathBetweenPoints(sf::Vector3f from, sf::Vector3f to) override;
};
