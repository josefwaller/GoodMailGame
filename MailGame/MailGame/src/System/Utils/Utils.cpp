#include "Utils.h"

sf::Sprite Utils::setupBuildingSprite(sf::Sprite spr, bool isBuilding) {
	// Orient the tiles from the center of where the tile is
	// Note that this is not neccessarily the center of the sprite, as buildings are taller sprites
	// with the actual center of the tile they're on is in the bottom half
	spr.setOrigin(spr.getLocalBounds().width / 2, spr.getLocalBounds().height - 64);
	return spr;
}
