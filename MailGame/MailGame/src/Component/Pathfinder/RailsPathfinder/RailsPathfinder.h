#pragma once
#include "Component/Pathfinder/Pathfinder.h"

class RailsPathfinder : public Pathfinder {
public:
	virtual std::vector<sf::Vector2f> findPathBetweenPoints(sf::Vector2f from, sf::Vector2f to) override;
};
