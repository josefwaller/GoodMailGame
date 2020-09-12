#include "AirPathfinder.h"
#include "Entity/Entity.h"
#include "Component/Transform/Transform.h"
#include "System/IsoRotation/IsoRotation.h"

std::vector<sf::Vector3f> AirPathfinder::findPathBetweenPoints(sf::Vector3f start, sf::Vector3f end) {
	std::vector<sf::Vector3f> path;
	// Add the start
	path.push_back(start);
	path.push_back(end);
	return path;
}
