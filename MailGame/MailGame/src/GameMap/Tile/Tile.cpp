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

bool Tile::canGetLock() {
	if (this->isLocked) {
		return false;
	}

	for (Railway r : this->railways) {
		if (r.getIsLocked()) {
			return false;
		}
	}

	return true;
}
bool Tile::canGetRailwayLock(Railway rail) {
	for (Railway r : this->railways) {
		if (!r.getIsLocked()) {
			continue;
		}
		// If the railway is not straight
		if (rail.getFrom().getReverse() != rail.getTo()) {
			// If the two railwasy do not overlap
			IsoRotation one = rail.getFrom().getReverse();
			IsoRotation two = rail.getTo().getReverse();
			if ((one == r.getFrom() && two == r.getTo()) || (one == r.getTo() && two == r.getFrom())) {
				continue;
			}
		}
		return false;
	}
	return true;
}

void Tile::getRailwayLock(Railway rail) {
	auto r = std::find_if(this->railways.begin(), this->railways.end(), [&rail](Railway r) { return r.getFrom() == rail.getFrom() && r.getTo() == rail.getTo(); });
	if (r == this->railways.end()) {
		throw std::runtime_error("Cannot release lock on railway that does not exist");
	}
	return r->getLock();
}
void Tile::releaseRailwayLock(Railway rail) {
	auto r = std::find_if(this->railways.begin(), this->railways.end(), [&rail](Railway r) { return r.getFrom() == rail.getFrom() && r.getTo() == rail.getTo(); });
	if (r == this->railways.end()) {
		throw std::runtime_error("Cannot get lock on railway that does not exist");
	}
	return r->releaseLock();
}