#include "AirPathfinder.h"
#include "Entity/Entity.h"
#include "Component/Transform/Transform.h"
#include "System/IsoRotation/IsoRotation.h"

std::vector<RoutePoint> AirPathfinder::findPathBetweenPoints(
	sf::Vector3f start,
	sf::Vector3f end,
	gtime_t departTime,
	float speed) {
	std::vector<RoutePoint> path;
	// Add the start
	path.push_back(RoutePoint(start, departTime, speed, 0.0f));
	path.push_back(RoutePoint(end, departTime + this->getTimeBetween(start, end, speed), speed, 0.0f));
	return path;
}