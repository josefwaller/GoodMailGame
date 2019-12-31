#pragma once
#include <memory>
// Forward declarations for speed
class Transform;
class Renderer;
class Game;

class Entity {
public:
	static std::shared_ptr<Entity> createEntity(
		Game* g,
		Transform* t = nullptr,
		Renderer* r = nullptr
	);
	// Components
	std::shared_ptr<Transform> transform;
	std::shared_ptr<Renderer> renderer;
	
	Game* getGame();
private:
	Entity(Game* g);
	Game* game;
};
