#pragma once
#include "System/SaveData/SaveData.h"

// The road on a tile
struct Road {
	// Whether it has road going in any of the 4 directions
	bool hasNorth;
	bool hasEast;
	bool hasSouth;
	bool hasWest;
	Road() : hasNorth(false), hasEast(false), hasSouth(false), hasWest(false) {}
	Road(SaveData rd) {
		hasNorth = (rd.getValue("hasNorth") == "1");
		hasSouth = (rd.getValue("hasSouth") == "1");
		hasEast = (rd.getValue("hasEast") == "1");
		hasWest = (rd.getValue("hasWest") == "1");
	}

	SaveData getSaveData() {
		SaveData rd("Road");
		rd.addValue("hasNorth", std::to_string(hasNorth));
		rd.addValue("hasEast", std::to_string(hasEast));
		rd.addValue("hasSouth", std::to_string(hasSouth));
		rd.addValue("hasWest", std::to_string(hasWest));
		return rd;
	}
};
