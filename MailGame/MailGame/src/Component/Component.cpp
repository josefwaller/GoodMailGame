#include "Component/Component.h"
#include "System/SaveData/SaveData.h"
#include <stdexcept>

void Component::setEntity(std::shared_ptr<Entity> e) {
	if (this->entity.lock()) {
		throw std::invalid_argument("Cannot set the entity of a component which already has an entity");
	}
	this->entity = e;
}
std::shared_ptr<Entity> Component::getEntity() {
	return this->entity.lock();
}
SaveData Component::getSaveData() {
	return SaveData("Component");
}
