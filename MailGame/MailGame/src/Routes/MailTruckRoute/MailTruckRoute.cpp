#include "MailTruckRoute.h"
#include "System/SaveData/SaveData.h"

SaveData mailTruckRouteToSaveData(MailTruckRoute route) {
	SaveData sd("MailTruckRoute");
	sd.addValue("isDelivering", route.isDelivering);
	sd.addValuesFrom(routeToSaveData(route));
	sd.addValue("numStops", route.stops.size());
	// Add the stops
	for (auto it = route.stops.begin(); it != route.stops.end(); it++) {
		SaveData d = mailTruckRouteStopToSaveData(*it);
		d.addValue("index", (it - route.stops.begin()));
		sd.addData(d);
	}
	return sd;
}
MailTruckRoute saveDataToMailTruckRoute(SaveData data) {
	bool isDelivering = data.getValue("isDelivering") == "1";
	Route route = saveDataToRoute(data);
	MailTruckRoute r(isDelivering, route.departTime);
	r.stops.resize(std::stoull(data.getValue("numStops")));
	for (SaveData d : data.getDatas()) {
		r.stops[std::stoull(d.getValue("index"))] = saveDataToMailTruckRouteStop(d);
	}
	return r;
}
SaveData mailTruckRouteStopToSaveData(MailTruckRouteStop stop) {
	SaveData stopData("MailTruckRouteStop");
	if (stop.target.has_value()) {
		sf::Vector2i target = stop.target.value();
		stopData.addValue("hasTarget", true);
		stopData.addValue("x", target.x);
		stopData.addValue("y", target.y);
	}
	else {
		stopData.addValue("hasTarget", false);
	}
	return stopData;
}
MailTruckRouteStop saveDataToMailTruckRouteStop(SaveData data) {
	MailTruckRouteStop stop;
	if (data.getValue("hasTarget") == "1") {
		stop.target = sf::Vector2i(
			std::stoi(data.getValue("x")),
			std::stoi(data.getValue("y"))
		);
	}
	return stop;
}