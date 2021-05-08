#pragma once
#include "System/IsoRotation/IsoRotation.h"
#include <vector>

// The railway on a tile
class Railway {
public:
	// The two directions the railway goes through
	IsoRotation from;
	IsoRotation to;
	// Whether the railway is a station or not
	bool isStation;
	Railway(IsoRotation from, IsoRotation to, bool isStation = false) : from(from), to(to), isStation(isStation) {}
	/**
	 *Get the connected tiles from the ingoing direction provided
	 * ingoingDirection is the direction the train/etc is coming from, relative to the tile
	 * i.e. if the ingoinDirection is south, the train is coming from the tile below this one
	 */
	std::vector<IsoRotation> getOutgoingDirections(IsoRotation ingoingDirection);
};
