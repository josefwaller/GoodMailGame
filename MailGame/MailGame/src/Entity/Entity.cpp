#include "Entity\Entity.h"
#include "Component/Component.h"
#include "Component/Transform/Transform.h"
#include "Component/Renderer/Renderer.h"


std::shared_ptr<Entity> Entity::createEntity(
	Game* g,
	EntityTag tag,
	Transform* t,
	Renderer* r) {

	// Create the entity
	std::shared_ptr<Entity> e(new Entity(g, tag));
	// Set components
	e->transform = std::shared_ptr<Transform>(t);
	t->setEntity(e);
	e->renderer = std::shared_ptr<Renderer>(r);
	r->setEntity(e);

	// Return built entity
	return e;

}
Entity::Entity(Game* g, EntityTag tag) {
	this->game = g;
	this->tag = tag;
}

Game* Entity::getGame() {
	return this->game;
}
