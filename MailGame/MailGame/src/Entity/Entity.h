#pragma once
#include <memory>
// Forward declarations for speed
class Transform;
class Renderer;
class Game;

class Entity {
public:
	Entity(Game* g);
	// Components
	std::shared_ptr<Transform> transform;
	std::shared_ptr <Renderer> renderer;
	
	Game* getGame();
private:
	Game* game;
};
