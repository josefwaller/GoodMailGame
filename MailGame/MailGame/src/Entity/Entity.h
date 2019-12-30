#pragma once
#include <memory>
// Forward declarations for speed
class Transform;
class Game;

class Entity {
public:
	Entity(Game* g);
	std::weak_ptr<Transform> transform;
	
	Game* getGame();
private:
	Game* game;
};
