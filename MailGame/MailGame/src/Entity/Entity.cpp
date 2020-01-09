#include "Entity\Entity.h"
#include "Component/Component.h"
#include "Component/Transform/Transform.h"
#include "Component/Renderer/Renderer.h"
#include "Component/Controller/Controller.h"

size_t Entity::entityId = 0;

std::shared_ptr<Entity> Entity::createEntity(
	Game* g,
	EntityTag tag,
	Transform* t,
	Renderer* r,
	Controller* c) {

	// Create the entity
	std::shared_ptr<Entity> e(new Entity(g, tag));
	// Set components
	if (t) {
		e->transform = std::shared_ptr<Transform>(t);
		t->setEntity(e);
	}
	if (r) {
		e->renderer = std::shared_ptr<Renderer>(r);
		r->setEntity(e);
	}
	if (c) {
		e->controller = std::shared_ptr<Controller>(c);
		c->setEntity(e);
	}

	// Return built entity
	return e;

}
Entity::Entity(Game* g, EntityTag t): tag(t), id(entityId++) {
	this->game = g;
}

size_t Entity::getId() {
	return this->id;
}

Game* Entity::getGame() {
	return this->game;
}
