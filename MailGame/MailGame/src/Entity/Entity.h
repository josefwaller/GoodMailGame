#pragma once
#include <memory>
#include "Entity/EntityTag/EntityTag.h"
// Forward declarations for speed
class Transform;
class Renderer;
class Controller;
class Game;

class Entity {
public:
	static std::shared_ptr<Entity> createEntity(
		Game* g,
		EntityTag tag,
		Transform* t = nullptr,
		Renderer* r = nullptr,
		Controller* c = nullptr
	);

	EntityTag tag;
	// Components
	std::shared_ptr<Transform> transform;
	std::shared_ptr<Renderer> renderer;
	std::shared_ptr<Controller> controller;
	
	Game* getGame();
private:
	Entity(Game* g, EntityTag t);
	Game* game;
};
