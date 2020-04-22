#include "TransitRoute.h"
#include "System/SaveData/SaveData.h"
#include "Entity/Entity.h"

unsigned long long TransitRouteStop::STOP_ID = 0;
unsigned long long TransitRoute::ROUTE_ID = 0;

SaveData transitRouteToSaveData(TransitRoute route) {
	SaveData sd = SaveData("TransitRoute");
	sd.addValue("departureTime", route.departureTime);
	sd.addValue("id", route.id);
	for (TransitRouteStop stop : route.stops) {
		sd.addData(transitRouteStopToSaveData(stop));
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
