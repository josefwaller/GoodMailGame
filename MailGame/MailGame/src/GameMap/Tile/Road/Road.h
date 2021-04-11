#pragma once
#include "System/SaveData/SaveData.h"
#include "System/IsoRotation/IsoRotation.h"
#include "Constants.h"

// The road on a tile
struct Road {
	// Whether it has road going in any of the 4 directions
	bool hasNorth;
	bool hasEast;
	bool hasSouth;
	bool hasWest;
	Road() : hasNorth(false), hasEast(false), hasSouth(false), hasWest(false) {}
	// Initialize a road that just points in a given direction
	Road(IsoRotation dir) :
		hasNorth(dir.getRotation() == IsoRotation::NORTH),
		hasEast(dir.getRotation() == IsoRotation::EAST),
		hasWest(dir.getRotation() == IsoRotation::WEST),
		hasSouth(dir.getRotation() == IsoRotation::SOUTH) {}
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
	std::vector<sf::Vector3i> getConnectedTiles(sf::Vector3i tile) {
		std::vector<sf::Vector3i> toReturn;
		if (this->hasNorth) {
			toReturn.push_back(tile + sf::Vector3i(0, -1, 0));
		}
		if (this->hasSouth) {
			toReturn.push_back(tile + sf::Vector3i(0, 1, 0));
		}
		if (this->hasEast) {
			toReturn.push_back(tile + sf::Vector3i(1, 0, 0));
		}
		if (this->hasWest) {
			toReturn.push_back(tile + sf::Vector3i(-1, 0, 0));
		}
		return toReturn;
	}
};
