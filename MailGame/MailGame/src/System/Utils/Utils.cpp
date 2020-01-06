#include "Utils.h"

sf::Sprite Utils::setupBuildingSprite(sf::Sprite spr) {
	spr.setOrigin(spr.getLocalBounds().width / 2, spr.getLocalBounds().height - 32);
	return spr;
}
