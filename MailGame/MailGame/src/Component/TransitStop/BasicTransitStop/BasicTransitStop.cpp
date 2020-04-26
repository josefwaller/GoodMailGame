#include "BasicTransitStop.h"
#include "Component/Transform/Transform.h"
#include "Entity/Entity.h"

sf::Vector3f BasicTransitStop::getTransitLocation() {
	return this->getEntity()->transform->getPosition();
}
