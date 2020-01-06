#pragma once
#include <map>
#include <functional>
#include <memory>
#include <SFML/System/Vector2.hpp>
#include "Entity/EntityTag/EntityTag.h"
#include "System/IsoRotation/IsoRotation.h"

class Entity;
class Game;

// Class used to aid in the player buildings things
// Contains all the recipes used to build things (i.e. post offices)
class Construction {
public:
	struct Recipe {
		std::function<std::shared_ptr<Entity>(Game* g, sf::Vector2f pos, IsoRotation rot)> createFunction;
		std::function<bool(Game* g, sf::Vector2f pos, IsoRotation rot)> positionIsValid;
	};
	static std::map<EntityTag, Recipe> recipes;
};
