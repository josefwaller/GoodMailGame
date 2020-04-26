#include "BuildingTransitStop.h"
#include "Entity/Entity.h"
#include "Component/Transform/Transform.h"

sf::Vector3f BuildingTransitStop::getTransitLocation() {
	auto trans = this->getEntity()->transform;
	return trans->getPosition() + trans->getRotation().getUnitVector3D();
}
