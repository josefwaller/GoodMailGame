#include "IsoSprite.h"

#define SPRS f(NORTH, north) \
f(NORTH_EAST, northEast) \
f(EAST, east) \
f(SOUTH_EAST, southEast) \
f(SOUTH, south) \
f(SOUTH_WEST, southWest) \
f(WEST, west) \
f(NORTH_WEST, northWest)

IsoSprite::IsoSprite(sf::Sprite n, sf::Sprite e, sf::Sprite s, sf::Sprite w, sf::Color color) : north(n), northEast(n), east(e), southEast(s), south(s), southWest(s), west(w), northWest(n) {
#define f(x, y) this->y.setColor(color);
	SPRS
#undef f
};
sf::Sprite IsoSprite::getSprite(IsoRotation rot) {
#define f(x, y) case IsoRotation::x: return this->y;
	switch (rot.getRotation()) {
		SPRS
	default: throw std::runtime_error("Invalid IsoRotation provided to IsoSprite!");
	}
#undef f
}

#undef SPRS