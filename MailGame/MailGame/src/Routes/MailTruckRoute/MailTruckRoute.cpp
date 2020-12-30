#include "MailTruckRoute.h"
#include "System/SaveData/SaveData.h"

SaveData MailTruckRoute::getSaveData() {
	SaveData sd("MailTruckRoute");
	sd.addValuesFrom(Route::getSaveData());
	sd.addValue("isDelivering", this->isDelivering);
	sd.addValue("numStops", this->stops.size());
	// Add the stops
	for (auto it = this->stops.begin(); it != this->stops.end(); it++) {
		SaveData d = mailTruckRouteStopToSaveData(*it);
		d.addValue("index", (it - this->stops.begin()));
		sd.addData(d);
	}
	return sd;
}
MailTruckRoute::MailTruckRoute(SaveData data) : Route(data) {
	bool isDelivering = data.getValue("isDelivering") == "1";
	this->stops.resize(std::stoull(data.getValue("numStops")));
	for (SaveData d : data.getDatas()) {
		this->stops[std::stoull(d.getValue("index"))] = saveDataToMailTruckRouteStop(d);
	}
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