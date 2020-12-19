#include "Entity\Entity.h"
#include "Component/Component.h"
#include "Component/ComponentType/ComponentType.h"
#include "Component/Transform/Transform.h"
#include "Component/Renderer/Renderer.h"
#include "Component/Controller/Controller.h"
#include "Component/ClickBox/ClickBox.h"
#include "Component/MailContainer/MailContainer.h"
#include "Component/TransitStop/TransitStop.h"
#include "Component/Pathfinder/Pathfinder.h"
#include "System/SaveData/SaveData.h"

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
	// Utility macro for setting components
#define SET_COMPONENT(var, field, cast) \
if (var) { \
	e->field = std::shared_ptr<cast>(var); \
	var->setEntity(e); \
}
	// Set components
	SET_COMPONENT(t, transform, Transform);
	SET_COMPONENT(r, renderer, Renderer);
	SET_COMPONENT(c, controller, Controller);
	SET_COMPONENT(cb, clickBox, ClickBox);
	SET_COMPONENT(m, mailContainer, MailContainer);
	SET_COMPONENT(ts, transitStop, TransitStop);
	SET_COMPONENT(p, pathfinder, Pathfinder);

#undef SET_COMPONENT
	// Return built entity
	return e;

}
Entity::Entity(Game* g, EntityTag t): tag(t), id(entityId++) {
	this->game = g;
}

size_t Entity::getId() {
	return this->id;
}

std::string Entity::getName() {
	return entityTagToString(this->tag) + " " + std::to_string(this->getId());
}

Game* Entity::getGame() {
	return this->game;
}

SaveData Entity::getSaveData() {
	SaveData data(entityTagToString(this->tag));
#define ADD_DATA(var) \
if (this->var) { \
	std::optional<SaveData> d = this->var->getSaveData(); \
	if (d.has_value()) \
		data.addData(d.value()); \
}

	ADD_DATA(transform);
	ADD_DATA(renderer);
	ADD_DATA(controller);
	ADD_DATA(clickBox);
	ADD_DATA(mailContainer);
	ADD_DATA(transitStop);
	ADD_DATA(pathfinder);

#undef ADD_DATA
	data.addValue("id", std::to_string(this->id));
	return data;
}

void Entity::fromSaveData(SaveData data) {
	this->id = std::stoull(data.getValue("id"));
	Entity::entityId = std::max(Entity::entityId, this->id + 1);
}

#define LOAD_COMPONENT(compType, var) case ComponentType::compType: \
	this->var->fromSaveData(d); \
	break;
void Entity::loadComponentsFromSaveData(SaveData data) {
	for (SaveData d : data.getDatas()) {
		switch (strToComponentType(d.getName())) {
			LOAD_COMPONENT(Transform, transform)
			LOAD_COMPONENT(Controller, controller)
			LOAD_COMPONENT(MailContainer, mailContainer)
			LOAD_COMPONENT(TransitStop, transitStop)
		default: break; // Todo: Do something here
		}
	}
}
#undef LOAD_COMPONENT
