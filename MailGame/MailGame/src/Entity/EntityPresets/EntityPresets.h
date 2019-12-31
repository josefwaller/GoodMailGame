#pragma once
#include <memory>
#include <SFML/System/Vector2.hpp>

class Entity;
class Game;

class EntityPresets {
public:
	static std::shared_ptr<Entity> building(Game* g, sf::Vector2f pos, float rotation);
};
