#pragma once
#include <string>

const int WINDOW_WIDTH = 1920;
const int WINDOW_HEIGHT = 1080;

// Type declarations
// Game time
typedef unsigned long long gtime_t;
// Ids
typedef unsigned long long id_t;
// The in-game hour
typedef unsigned int hour_t;
// In-game money
typedef long money_t;
// SaveData keys
typedef std::string sdkey_t;

namespace SaveKeys {
	const sdkey_t GAMEMAP = "GameMap";
	const sdkey_t TILE = "tile";
	const sdkey_t X = "x";
	const sdkey_t Y = "y";
	const sdkey_t POSTAL_CODE = "postalCode";
	const sdkey_t TYPE = "type";
	const sdkey_t BUILDING = "building";
	const sdkey_t RAILWAY = "Railway";
	const sdkey_t TO = "to";
	const sdkey_t FROM = "from";
	const sdkey_t IS_STATION = "isStation";
	const sdkey_t ROAD = "Road";
	const sdkey_t HAS_NORTH = "hasNorth";
	const sdkey_t HAS_EAST = "hasEast";
	const sdkey_t HAS_SOUTH = "hasSouth";
	const sdkey_t HAS_WEST = "hasWest";
	const sdkey_t AIRPLANE_ROAD = "AirplaneRoad";
	const sdkey_t IS_RUNWAY = "isRunway";
}
