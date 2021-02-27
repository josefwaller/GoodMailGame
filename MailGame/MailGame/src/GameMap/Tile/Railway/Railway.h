#pragma once
#include "System/IsoRotation/IsoRotation.h"

// The railway on a tile
struct Railway {
	// The two directions the railway goes through
	IsoRotation from;
	IsoRotation to;
	// Whether the railway is a station or not
	bool isStation;
	Railway(IsoRotation from, IsoRotation to, bool isStation = false) : from(from), to(to), isStation(isStation) {}
};
