#pragma once
#include <map>
#include <functional>
#include <memory>
#include <SFML/System/Vector2.hpp>
#include "Entity/EntityTag/EntityTag.h"

class Entity;
class Game;

// Class used to aid in the player buildings things
// Contains all the recipes used to build things (i.e. post offices)
class Construction {
public:
	struct Recipe {
		std::function<std::shared_ptr<Entity>(sf::Vector2f pos, Game* g)> createFunction;
	};
	const static std::map<EntityTag, Recipe> recipes;
};
