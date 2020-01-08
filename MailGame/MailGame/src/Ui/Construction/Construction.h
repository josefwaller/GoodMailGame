#pragma once
#include <map>
#include <functional>
#include <memory>
#include <SFML/System/Vector2.hpp>
#include "Entity/EntityTag/EntityTag.h"
#include "System/IsoRotation/IsoRotation.h"
#include <SFML/Graphics/Sprite.hpp>

class Entity;
class Game;

// Class used to aid in the player buildings things
// Contains all the recipes used to build things (i.e. post offices)
class Construction {
public:
	struct Recipe {
		// Method to actually create the entity, does not check if it is valid or not
		std::function<std::shared_ptr<Entity>(Game* g, sf::Vector2f pos, IsoRotation rot)> createFunction;
		// Method to check if the entity position is valid
		std::function<bool(Game* g, sf::Vector2f pos, IsoRotation rot)> positionIsValid;
		// Method to draw the hovering sprite when building something
		// isValid is whether the sprite is valid or not, as determined by positionIsValid
		// in case the sprite should be drawn red or something
		// Here is where it would snap to grid, etc
		std::function<sf::Sprite(Game* g, sf::Vector2f pos, IsoRotation rot, bool isValid)> getRenderSprite;

	};
	static std::map<EntityTag, Recipe> recipes;
};
