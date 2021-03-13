#pragma once
#include "System/SaveData/SaveData.h"
#include "Constants.h"

// The road on a tile
struct Road {
	// Whether it has road going in any of the 4 directions
	bool hasNorth;
	bool hasEast;
	bool hasSouth;
	bool hasWest;
	Road() : hasNorth(false), hasEast(false), hasSouth(false), hasWest(false) {}
	Road(SaveData rd) {
		using namespace SaveKeys;
		hasNorth = rd.getBool(HAS_NORTH);
		hasEast = rd.getBool(HAS_EAST);
		hasSouth = rd.getBool(HAS_SOUTH);
		hasWest = rd.getBool(HAS_WEST);
	}

	SaveData getSaveData() {
		using namespace SaveKeys;
		SaveData rd(ROAD);
		rd.addBool(HAS_NORTH, hasNorth);
		rd.addBool(HAS_EAST, hasEast);
		rd.addBool(HAS_SOUTH, hasSouth);
		rd.addBool(HAS_WEST, hasWest);
		return rd;
	}
};
