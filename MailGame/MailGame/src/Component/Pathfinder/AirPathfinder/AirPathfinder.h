#pragma once
#include "Component/Pathfinder/Pathfinder.h"

class AirPathfinder : public Pathfinder {
public:
	virtual std::vector<SpeedPoint> findPathBetweenPoints(
		sf::Vector3f start,
		sf::Vector3f end,
		gtime_t departTime,
		float speed) override;
};
