#include "TransitRoute.h"
#include "System/SaveData/SaveData.h"
#include "Entity/Entity.h"
#include "Game/Game.h"

unsigned long long TransitRouteStop::STOP_ID = 0;

TransitRoute::TransitRoute(hour_t time, VehicleModel m, CargoCarModel s, unsigned int num) : Route(time, m, s, num) {}
TransitRoute::TransitRoute(hour_t time, VehicleModel m) : Route(time, m) {}

TransitRoute::TransitRoute(SaveData data, Game* g) : Route(data) {
	this->stops.resize(std::stoull(data.getValue("numStops")));
	for (SaveData d : data.getDatas()) {
		size_t index = std::stoull(d.getValue("index"));
		this->stops[index] = saveDataToTransitRouteStop(g, d);
	}
}

SaveData TransitRoute::getSaveData() {
	SaveData sd("TransitRoute");
	sd.addValuesFrom(Route::getSaveData());
	sd.addValue("numStops", this->stops.size());
	for (size_t i = 0; i < this->stops.size(); i++) {
		TransitRouteStop stop = this->stops[i];
		SaveData d = transitRouteStopToSaveData(stop);
		d.addValue("index", i);
		sd.addData(d);
	}
	return sd;
}

SaveData transitRouteStopToSaveData(TransitRouteStop stop) {
	SaveData sd("TransitRouteStop");
	if (stop.target.lock()) {
		sd.addValue("hasTarget", true);
		sd.addValue("targetId", stop.target.lock()->getId());
	}
	else {
		sd.addValue("hasTarget", false);
	}
	// Add pick up/drop offs
	for (long long code : stop.toPickUp) {
		SaveData p("PickUp");
		p.addValue("code", code);
		sd.addData(p);
	}
	for (long long code : stop.toDropOff) {
		SaveData d("DropOff");
		d.addValue("code", code);
		sd.addData(d);
	}
	return sd;
}
TransitRouteStop saveDataToTransitRouteStop(Game* g, SaveData data) {
	TransitRouteStop stop;
	if (data.getValue("hasTarget") == "1") {
		stop.target = g->getEntityById(std::stoull(data.getValue("targetId")));
	}
	// Set pick up/drop off
	for (SaveData d : data.getDatas()) {
		if (d.getName() == "PickUp") {
			stop.toPickUp.push_back(std::stoull(d.getValue("code")));
		}
		else if (d.getName() == "DropOff") {
			stop.toDropOff.push_back(std::stoull(d.getValue("code")));
		}
	}
	return stop;
}