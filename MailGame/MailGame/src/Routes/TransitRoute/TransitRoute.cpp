#include "TransitRoute.h"
#include "System/SaveData/SaveData.h"
#include "Entity/Entity.h"
#include "Game/Game.h"

unsigned long long TransitRouteStop::STOP_ID = 0;

TransitRoute::TransitRoute(hour_t time, VehicleModel m, CargoCarModel s, unsigned int num) : Route(time, m, s, num) {}
TransitRoute::TransitRoute(hour_t time, VehicleModel m) : Route(time, m) {}

TransitRoute::TransitRoute(SaveData data, Game* g) : Route(data) {
	using namespace SaveKeys;
	this->stops.resize(data.getSizeT(NUM_STOPS));
	for (SaveData d : data.getDatas()) {
		this->stops[d.getSizeT(INDEX)] = saveDataToTransitRouteStop(g, d);
	}
}

SaveData TransitRoute::getSaveData() {
	using namespace SaveKeys;
	SaveData sd(TRANSIT_ROUTE);
	sd.addValuesFrom(Route::getSaveData());
	sd.addSizeT(NUM_STOPS, this->stops.size());
	for (size_t i = 0; i < this->stops.size(); i++) {
		TransitRouteStop stop = this->stops[i];
		SaveData d = transitRouteStopToSaveData(stop);
		d.addSizeT(INDEX, i);
		sd.addData(d);
	}
	return sd;
}

SaveData transitRouteStopToSaveData(TransitRouteStop stop) {
	using namespace SaveKeys;
	SaveData sd(TRANSIT_ROUTE_STOP);
	if (stop.target.lock()) {
		sd.addBool(HAS_TARGET, true);
		sd.addSizeT(TARGET, stop.target.lock()->getId());
	}
	else {
		sd.addBool(HAS_TARGET, false);
	}
	// Add pick up/drop offs
	for (long long code : stop.toPickUp) {
		SaveData p(PICK_UP);
		p.addSizeT(POSTAL_CODE, code);
		sd.addData(p);
	}
	for (long long code : stop.toDropOff) {
		SaveData d(DROP_OFF);
		d.addValue(POSTAL_CODE, code);
		sd.addData(d);
	}
	return sd;
}
TransitRouteStop saveDataToTransitRouteStop(Game* g, SaveData data) {
	using namespace SaveKeys;
	TransitRouteStop stop;
	if (data.getBool(HAS_TARGET)) {
		stop.target = g->getEntityById(data.getSizeT(TARGET));
	}
	// Set pick up/drop off
	for (SaveData d : data.getDatas()) {
		if (d.getName() == PICK_UP) {
			stop.toPickUp.push_back(d.getSizeT(POSTAL_CODE));
		}
		else if (d.getName() == DROP_OFF) {
			stop.toDropOff.push_back(d.getSizeT(POSTAL_CODE));
		}
	}
	return stop;
}