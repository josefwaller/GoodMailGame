#pragma once
#include <memory>
#include "Entity/EntityTag/EntityTag.h"
// Forward declarations for speed
class Transform;
class Renderer;
class Controller;
class ClickBox;
class MailContainer;
class TransitStop;
class Pathfinder;
class Game;
class SaveData;

class Entity {
public:
	static std::shared_ptr<Entity> createEntity(
		Game* g,
		EntityTag tag,
		Transform* t = nullptr,
		Renderer* r = nullptr,
		Controller* c = nullptr,
		ClickBox* cb = nullptr,
		MailContainer* m = nullptr,
		TransitStop* ts = nullptr,
		Pathfinder* p = nullptr
	);
	// The tag of the entity
	EntityTag tag;
	// Components
	std::shared_ptr<Transform> transform;
	std::shared_ptr<Renderer> renderer;
	std::shared_ptr<Controller> controller;
	std::shared_ptr<ClickBox> clickBox;
	std::shared_ptr<MailContainer> mailContainer;
	std::shared_ptr<TransitStop> transitStop;
	std::shared_ptr<Pathfinder> pathfinder;
	// Get the id
	size_t getId();
	// Get the game
	Game* getGame();
	// Get the save data for this entities
	SaveData getSaveData();
	// Overwrite the entity with the save data given
	void fromSaveData(SaveData d);
	// Set connections between components/entities using id
	// Can assume the correct ids have been set here
	void loadComponentsFromSaveData(SaveData d);
private:
	// The ID of the entity
	size_t id;
	static size_t entityId;
	// Constructor
	// Entities should be make using createEntity
	Entity(Game* g, EntityTag t);
	// The game
	Game* game;
};
