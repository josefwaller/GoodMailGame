#pragma once
#include "Component/Pathfinder/Pathfinder.h"

class RailsPathfinder : public Pathfinder {
public:
	virtual std::vector<sf::Vector3f> findPathBetweenPoints(sf::Vector3f from, sf::Vector3f to) override;
};
