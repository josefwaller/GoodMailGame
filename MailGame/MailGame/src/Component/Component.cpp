#include "Component/Component.h"

Component::Component(std::weak_ptr<Entity> e) {
	this->entity = e;
}
std::shared_ptr<Entity> Component::getEntity() {
	return this->entity.lock();
}