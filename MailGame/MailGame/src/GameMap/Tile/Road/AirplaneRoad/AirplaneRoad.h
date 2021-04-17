#pragma once
#include "../Road.h"

struct AirplaneRoad : public Road {
	bool isRunway;
	AirplaneRoad() : Road(), isRunway(false) {};
	AirplaneRoad(IsoRotation rot) : Road(rot), isRunway(false) {}
	AirplaneRoad(SaveData data) : Road(data), isRunway(data.getBool(SaveKeys::IS_RUNWAY)) {}
	SaveData getSaveData() {
		using namespace SaveKeys;
		SaveData d(AIRPLANE_ROAD, Road::getSaveData());
		d.addBool(IS_RUNWAY, isRunway);
		return d;
	}
};
