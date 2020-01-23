#pragma once
#include <map>
#include <functional>
#include <memory>
#include <SFML/System/Vector2.hpp>
#include "Entity/EntityTag/EntityTag.h"
#include "System/IsoRotation/IsoRotation.h"
#include <SFML/Graphics/Sprite.hpp>
#include "ConstructionRecipe//ConstructionRecipe.h"

class Entity;
class Game;

// Class used to aid in the player buildings things
// Contains all the recipes used to build things (i.e. post offices)
class Construction {
public:
	static std::map<EntityTag, ConstructionRecipe> recipes;
};
