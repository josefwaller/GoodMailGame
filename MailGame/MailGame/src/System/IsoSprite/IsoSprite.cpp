#include "IsoSprite.h"

IsoSprite::IsoSprite(sf::Sprite n, sf::Sprite e, sf::Sprite s, sf::Sprite w, sf::Color color) : north(n), east(e), south(s), west(w) {
	this->north.setColor(color);
	this->east.setColor(color);
	this->south.setColor(color);
	this->west.setColor(color);
};
sf::Sprite IsoSprite::getSprite(IsoRotation rot) {
	switch (rot.getRotation()) {
	case IsoRotation::NORTH: return north;
	case IsoRotation::EAST: return east;
	case IsoRotation::SOUTH: return south;
	case IsoRotation::WEST: return west;
	default: throw std::runtime_error("Invalid IsoRotation provided to IsoSprite!");
	}
}