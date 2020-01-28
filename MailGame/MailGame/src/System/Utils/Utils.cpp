#include "Utils.h"

sf::Sprite Utils::setupBuildingSprite(sf::Sprite spr, bool isBuilding) {
	spr.setOrigin(spr.getLocalBounds().width / 2, 0);
	if (isBuilding) {
		spr.setOrigin(spr.getLocalBounds().width / 2, spr.getLocalBounds().height - 32 - 64);
	}
	return spr;
}
