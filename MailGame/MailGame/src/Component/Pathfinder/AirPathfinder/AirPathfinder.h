#pragma once
#include "Component/Pathfinder/Pathfinder.h"

class AirPathfinder : public Pathfinder {
public:
	virtual std::vector<sf::Vector3f> findPathBetweenPoints(sf::Vector3f start, sf::Vector3f end) override;
};
