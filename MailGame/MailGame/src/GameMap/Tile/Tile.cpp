#include "./Tile.h"

Tile::Tile(TileType t, id_t cityId) : postalCode(0), isLocked(false), cityId(cityId), railways({}), type(t) {
}

std::vector<IsoRotation> Tile::getOutgoingRailDirections(IsoRotation ingoingDirection) {
	// If the ingoingDirection is north, it is coming from below the tile, so hte direction we need to look for is south
	// Same for all other directions
	IsoRotation from = ingoingDirection;
	std::vector<IsoRotation> toReturn;
	for (Railway r : this->railways) {
		if (r.getFrom() == from) {
			toReturn.push_back(r.getTo());
		}
	}
	return toReturn;
}

void Tile::addRailway(Railway r) {
	// Make sure we don't have this railway anymore
	for (Railway rail : this->railways) {
		if (rail.getFrom() == r.getFrom() && rail.getTo() == r.getTo()) {
			return;
		}
	}
	this->railways.push_back(r);
}
void Tile::deleteRailways() {
	this->railways = {};
}
std::vector<Railway> Tile::getRailways() {
	return this->railways;
}