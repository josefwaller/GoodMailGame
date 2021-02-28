#pragma once
#include <memory>
#include <optional>
#include <map>
#include "System/IsoRotation/IsoRotation.h"
#include "Railway/Railway.h"
#include "Constants.h"
#include "./Road/Road.h"

// Forward declaration
class Entity;

// The three types of tiles
enum class TileType {
	Land,
	Water,
	OffMap
};

struct Tile {
	// The type of tile
	TileType type;
	// The building on the tile
	std::weak_ptr<Entity> building;
	// The postal code assigned to the tile
	long long postalCode;
	// The railway, if it has one
	// Keyed by the time it switches to that railway
	std::optional<std::map<hour_t, Railway>> railway;
	// The road, if it has one
	std::optional<Road> road;
	// The tile lock, currently just one for all transit types
	bool isLocked;

	Tile(TileType t = TileType::Land) : postalCode(0), isLocked(false) {
		type = t;
	}
	// Get the railway at a certain hour of the day
	std::optional<Railway> getRailwayAtHour(hour_t hour) {
		if (!railway.has_value()) {
			return {};
		}
		// TODO: This assumes that railways always have a set value at 0 (Midnight), which is subject to change
		std::map<hour_t, Railway> r = railway.value();
		hour_t prevHour = 0;
		bool prevHourSet = false;
		for (auto it = r.begin(); it != r.end(); it++) {
			if (!prevHourSet) {
				prevHour = it->first;
				prevHourSet = true;
			}
			else {
				if (getHourDiff(prevHour, hour) > getHourDiff(it->first, hour)) {
					prevHour = it->first;
				}
			}
		}
		return r.at(prevHour);
	}
	hour_t getHourDiff(hour_t from, hour_t to) {
		if (from <= to) {
			return to - from;
		}
		else {
			return 24 - (from - to);
		}
	}
};
