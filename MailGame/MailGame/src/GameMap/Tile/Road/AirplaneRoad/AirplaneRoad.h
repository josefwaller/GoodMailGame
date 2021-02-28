#pragma once
#include "../Road.h"

struct AirplaneRoad : public Road {
	bool isRunway;
	AirplaneRoad() : Road(), isRunway(false) {};
	AirplaneRoad(SaveData data) : Road(data), isRunway(data.getValue("isRunway") == "1") {}
	SaveData getSaveData() {
		SaveData d("AirplaneRoad");
		d.addValuesFrom(Road::getSaveData());
		d.addValue("isRunway", isRunway);
		return d;
	}
};
