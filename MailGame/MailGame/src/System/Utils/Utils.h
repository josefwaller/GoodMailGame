#pragma once
#include <SFML/Graphics/Sprite.hpp>

// Misc Utils that I couldn't think to put anywhere else
// Mostly temporary things that shouldn't make it into the final product
class Utils {
public:
	// Setup a sprite to be drawn as a building, since the spritesheet I'm using right now
	// has some extra ground for buildings
	static sf::Sprite setupBuildingSprite(sf::Sprite spr);
};
