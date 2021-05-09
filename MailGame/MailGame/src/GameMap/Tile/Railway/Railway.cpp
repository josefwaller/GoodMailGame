#include "Railway.h"
#include "Constants.h"
#include "System/SaveData/SaveData.h"
#include <algorithm>

Railway::Railway(SaveData data) {
	using namespace SaveKeys;
	for (SaveData d : data.getDatas()) {
		this->directions.push_back({ d.getIsoRotation(FROM), d.getIsoRotation(TO) });
	}
	this->isStation = data.getBool(IS_STATION);
}
std::vector<IsoRotation> Railway::getOutgoingDirections(IsoRotation ingoingDirection) {
	// If the ingoingDirection is north, it is coming from below the tile, so hte direction we need to look for is south
	// Same for all other directions
	IsoRotation from = ingoingDirection;
	std::vector<IsoRotation> toReturn;
	for (auto kv : this->directions) {
		if (kv.first == from) {
			toReturn.push_back(kv.second);
		}
	}
	return toReturn;
}

void Railway::addDirection(IsoRotation from, IsoRotation to) {
	if (std::find_if(this->directions.begin(), this->directions.end(), [from, to](auto kv) {
		return kv.first == from && kv.second == to;
		}) == this->directions.end()) {
		this->directions.push_back({ from ,to });
	}
}

std::vector<std::pair<IsoRotation, IsoRotation>> Railway::getDirections() {
	return this->directions;
}