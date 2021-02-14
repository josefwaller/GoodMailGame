#pragma once
#include "Component/Pathfinder/Pathfinder.h"

/*
 * A pathfinder that finds a path between two points by going along roads
 */
class RoadPathfinder : public Pathfinder {
public:
	virtual std::vector<SpeedPoint> findPathBetweenPoints(
		sf::Vector3f from,
		sf::Vector3f to,
		gtime_t departTime,
		float speed) override;
};
