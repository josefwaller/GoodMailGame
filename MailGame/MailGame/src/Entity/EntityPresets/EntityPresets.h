#pragma once
#include <memory>
#include <SFML/System/Vector2.hpp>
#include "System/IsoRotation/IsoRotation.h"

class Entity;
class Game;

class EntityPresets {
public:
	static std::shared_ptr<Entity> building(Game* g, sf::Vector2f pos, IsoRotation rot);
};
