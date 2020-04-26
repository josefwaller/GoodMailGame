#pragma once
#include <functional>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include "System/IsoRotation/IsoRotation.h"
#include "ConstructionLayout/ConstructionLayout.h"

class Entity;
class Game;

/*
 * Recipe for an entity that can only be rotated 90 degrees at a time,
 * and is snapped to grid
 * Should be every entity the play can build
 */
class ConstructionRecipe{
public:
	ConstructionRecipe();
	ConstructionRecipe(
		// The EntityPresets function to build this entity
		std::function<std::shared_ptr<Entity>(Game* g, sf::Vector3f pos, IsoRotation rot)> buildFunction,
		// The sprites to display, ordered by rotation
		std::vector<sf::Sprite> displaySprites,
		// The alyout required for a valid position for this building
		ConstructionLayout layout
	);
	bool isValid(Game* g, sf::Vector3f pos, IsoRotation rot);
	void renderConstructionSprite(Game* g, sf::Vector3f pos, IsoRotation rot, sf::RenderWindow* w);
	std::shared_ptr<Entity> buildRecipe(Game* g, sf::Vector3f pos, IsoRotation rot);
private:
	// The function to build the entity
	std::function<std::shared_ptr<Entity>(Game* g, sf::Vector3f pos, IsoRotation rot)> buildFunction;
	// The sprites to display
	std::vector<sf::Sprite> displaySprites;
	// The layout
	ConstructionLayout layout;
	// Get rounded coordinates
	sf::Vector3i roundCoords(sf::Vector3f pos);
};
