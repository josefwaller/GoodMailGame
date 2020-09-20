#include "Utils.h"
#include <math.h>

sf::Sprite Utils::setupBuildingSprite(sf::Sprite spr, bool isBuilding) {
	// Orient the tiles from the center of where the tile is
	// Note that this is not neccessarily the center of the sprite, as buildings are taller sprites
	// with the actual center of the tile they're on is in the bottom half
	spr.setOrigin(spr.getLocalBounds().width / 2, spr.getLocalBounds().height - 64);
	return spr;
}

float Utils::getVectorDistance(sf::Vector3f one, sf::Vector3f two) {
	return sqrtf(powf(one.x - two.x, 2) + powf(one.y - two.y, 2) + powf(one.z - two.z, 2));
}
