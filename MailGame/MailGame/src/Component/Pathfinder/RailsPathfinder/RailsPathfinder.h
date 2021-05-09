#pragma once
#include "Component/Pathfinder/Pathfinder.h"

class RailsPathfinder : public Pathfinder {
public:
	/*
	 * Find a path between two points, assuming it is on fromTile from any direction
	 */
	virtual std::vector<SpeedPoint> findPathBetweenPoints(
		sf::Vector3f from,
		sf::Vector3f to,
		gtime_t departTime,
		float speed) override;
};
