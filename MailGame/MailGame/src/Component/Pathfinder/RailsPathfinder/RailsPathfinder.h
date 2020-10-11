#pragma once
#include "Component/Pathfinder/Pathfinder.h"

class RailsPathfinder : public Pathfinder {
public:
	virtual std::vector<RoutePoint> findPathBetweenPoints(
		sf::Vector3f from,
		sf::Vector3f to,
		gtime_t departTime,
		float speed) override;
};
