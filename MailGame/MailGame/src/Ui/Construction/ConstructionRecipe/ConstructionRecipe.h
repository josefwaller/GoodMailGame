#pragma once
#include <functional>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include "System/IsoRotation/IsoRotation.h"

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
		std::function<std::shared_ptr<Entity>(Game* g, sf::Vector2f pos, IsoRotation rot)> buildFunction,
		// The sprites to display, ordered by rotation
		std::vector<sf::Sprite> displaySprites
	);
	bool isValid(Game* g, sf::Vector2f pos, IsoRotation rot);
	void renderConstructionSprite(Game* g, sf::Vector2f pos, IsoRotation rot, sf::RenderWindow* w);
	std::shared_ptr<Entity> buildRecipe(Game* g, sf::Vector2f pos, IsoRotation rot);
private:
	// The function to build the entity
	std::function<std::shared_ptr<Entity>(Game* g, sf::Vector2f pos, IsoRotation rot)> buildFunction;
	// The sprites to display
	std::vector<sf::Sprite> displaySprites;
	// Get rounded coordinates
	sf::Vector2i roundCoords(sf::Vector2f pos);
};
