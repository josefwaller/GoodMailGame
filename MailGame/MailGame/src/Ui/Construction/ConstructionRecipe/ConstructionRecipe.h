#pragma once
#include <functional>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include "System/IsoRotation/IsoRotation.h"
#include "ConstructionLayout/ConstructionLayout.h"
#include "System/IsoSprite/IsoSprite.h"
#include "TechTree/Technology/Technology.h"

class Entity;
class Game;

/*
 * Recipe for an entity that can only be rotated 90 degrees at a time,
 * and is snapped to grid
 * Should be every entity the play can build
 */
class ConstructionRecipe {
public:
	ConstructionRecipe(
		// The display name
		std::string displayName,
		// The EntityPresets function to build this entity
		std::function<std::shared_ptr<Entity>(Game * g, sf::Vector3f pos, IsoRotation rot)> buildFunction,
		// The sprites to display
		IsoSprite sprites,
		// The alyout required for a valid position for this building
		ConstructionLayout layout,
		// The technology required to build this entity
		Technology requiredTech = Technology::Default
	);
	bool isValid(Game* g, sf::Vector3f pos, IsoRotation rot);
	void renderConstructionSprite(Game* g, sf::Vector3f pos, IsoRotation rot, sf::RenderWindow* w);
	std::shared_ptr<Entity> buildRecipe(Game* g, sf::Vector3f pos, IsoRotation rot);
	Technology getRequiredTech();
	std::string getDisplayName();
private:
	// The function to build the entity
	std::function<std::shared_ptr<Entity>(Game * g, sf::Vector3f pos, IsoRotation rot)> buildFunction;
	// The sprites to display
	IsoSprite displaySprites;
	// The layout
	ConstructionLayout layout;
	// Get rounded coordinates
	sf::Vector3i roundCoords(sf::Vector3f pos);
	// Required tech
	Technology requiredTech;
	std::string displayName;
};
