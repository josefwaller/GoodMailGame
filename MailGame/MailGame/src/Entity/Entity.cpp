#include "Entity\Entity.h"
#include "Component/Component.h"
#include "Component/Transform/Transform.h"
#include "Component/Renderer/Renderer.h"


std::shared_ptr<Entity> Entity::createEntity(
	Game* g,
	Transform* t,
	Renderer* r) {

	// Create the entity
	std::shared_ptr<Entity> e(new Entity(g));
	// Set components
	e->transform = std::shared_ptr<Transform>(t);
	t->setEntity(e);
	e->renderer = std::shared_ptr<Renderer>(r);
	r->setEntity(e);

	// Return built entity
	return e;

}
Entity::Entity(Game* g) {
	this->game = g;
}

Game* Entity::getGame() {
	return this->game;
}