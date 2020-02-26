#pragma once
#include "Component/Pathfinder/Pathfinder.h"

/*
 * A pathfinder that finds a path between two points by going along roads
 */
class RoadPathfinder : public Pathfinder {
public:
	virtual std::vector<sf::Vector2f> findPathBetweenPoints(sf::Vector2f from, sf::Vector2f to) override;
};
