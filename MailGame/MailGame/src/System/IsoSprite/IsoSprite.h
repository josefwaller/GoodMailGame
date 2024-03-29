#pragma once
#include <SFML/Graphics/Sprite.hpp>
#include "System/IsoRotation/IsoRotation.h"
/*
 * Holds a sprite for all 4 IsoRotations
 */
class IsoSprite {
public:
	IsoSprite(sf::Sprite north,
		sf::Sprite east,
		sf::Sprite south,
		sf::Sprite west,
		sf::Color c = sf::Color::White);
	IsoSprite(sf::Sprite north,
		sf::Sprite northEast,
		sf::Sprite east,
		sf::Sprite southEast,
		sf::Sprite south,
		sf::Sprite southWest,
		sf::Sprite west,
		sf::Sprite northWest,
		sf::Color c = sf::Color::White);
	sf::Sprite getSprite(IsoRotation rot);
private:
	sf::Sprite north;
	sf::Sprite northEast;
	sf::Sprite east;
	sf::Sprite southEast;
	sf::Sprite south;
	sf::Sprite southWest;
	sf::Sprite west;
	sf::Sprite northWest;
};
