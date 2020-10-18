#include "TransitRoute.h"
#include "System/SaveData/SaveData.h"
#include "Entity/Entity.h"
#include "Game/Game.h"

unsigned long long TransitRouteStop::STOP_ID = 0;
unsigned long long TransitRoute::ROUTE_ID = 0;
SaveData transitRouteToSaveData(TransitRoute route) {
	SaveData sd = SaveData("TransitRoute");
	sd.addValue("departureTime", route.departureTime);
	sd.addValue("numStops", route.stops.size());
	sd.addValue("model", vehicleModelToString(route.model));
	for (size_t i = 0; i < route.stops.size(); i++) {
		TransitRouteStop stop = route.stops[i];
		SaveData d = transitRouteStopToSaveData(stop);
		d.addValue("index", i);
		sd.addData(d);
	}
	return sd;
}

TransitRoute saveDataToTransitRoute(Game* g, SaveData data) {
	TransitRoute route(std::stoi(data.getValue("departureTime")), stringToVehicleModel(data.getValue("model")));
	route.stops.resize(std::stoull(data.getValue("numStops")));
	for (SaveData d : data.getDatas()) {
		size_t index = std::stoull(d.getValue("index"));
		route.stops[index] = saveDataToTransitRouteStop(g, d);
	}
	return route;
}

SaveData transitRouteStopToSaveData(TransitRouteStop stop) {
	SaveData sd("TransitRouteStop");
	if (stop.target.lock()) {
		sd.addValue("hasTarget", true);
		sd.addValue("targetId", stop.target.lock()->getId());
		for (RoutePoint p : stop.points) {
			sd.addData(routePointToSaveData(p));
		}
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
		for (SaveData d : data.getDatas()) {
			stop.points.push_back(saveDataToRoutePoint(d));
		}
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
