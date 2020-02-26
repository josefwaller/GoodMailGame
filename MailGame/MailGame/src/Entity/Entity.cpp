#include "Entity\Entity.h"
#include "Component/Component.h"
#include "Component/Transform/Transform.h"
#include "Component/Renderer/Renderer.h"
#include "Component/Controller/Controller.h"
#include "Component/ClickBox/ClickBox.h"
#include "Component/MailContainer/MailContainer.h"
#include "Component/TransitStop/TransitStop.h"
#include "Component/Pathfinder/Pathfinder.h"

size_t Entity::entityId = 0;

std::shared_ptr<Entity> Entity::createEntity(
	Game* g,
	EntityTag tag,
	Transform* t,
	Renderer* r,
	Controller* c,
	ClickBox* cb,
	MailContainer* m,
	TransitStop* ts,
	Pathfinder* p) {

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
	if (cb) {
		e->clickBox = std::shared_ptr<ClickBox>(cb);
		cb->setEntity(e);
	}
	if (m) {
		e->mailContainer = std::shared_ptr<MailContainer>(m);
		m->setEntity(e);
	}
	if (ts) {
		e->transitStop = std::shared_ptr<TransitStop>(ts);
		ts->setEntity(e);
	}

	if (p) {
		e->pathfinder = std::shared_ptr<Pathfinder>(p);
		p->setEntity(e);
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
