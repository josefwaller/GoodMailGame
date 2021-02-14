#include "AirPathfinder.h"
#include "Entity/Entity.h"
#include "Component/Transform/Transform.h"
#include "System/IsoRotation/IsoRotation.h"

std::vector<SpeedPoint> AirPathfinder::findPathBetweenPoints(
	sf::Vector3f start,
	sf::Vector3f end,
	gtime_t departTime,
	float speed) {
	// This will eventually be more complicated
	return { start, end };
}