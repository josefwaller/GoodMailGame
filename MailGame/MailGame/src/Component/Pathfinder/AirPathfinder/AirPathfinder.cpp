#include "AirPathfinder.h"
#include "Entity/Entity.h"
#include "Component/Transform/Transform.h"
#include "System/IsoRotation/IsoRotation.h"

std::vector<sf::Vector3f> AirPathfinder::findPathBetweenPoints(sf::Vector3f start, sf::Vector3f end) {
	std::vector<sf::Vector3f> path;
	// Add the start
	path.push_back(start);
	// Add the point 5 tiles away at proper height
	path.push_back(
		start + 5.0f * this->getEntity()->transform->getRotation().getUnitVector3D() + sf::Vector3f(0, 0, 5.0f)
	);
	path.push_back(
		end + 5.0f * sf::Vector3f(1.0f, 0, 0) + sf::Vector3f(0, 0, 5.0f)
	);
	path.push_back(
		end
	);
	return path;
}
