#include "MailTruckRoute.h"
#include "System/SaveData/SaveData.h"

SaveData MailTruckRoute::getSaveData() {
	using namespace SaveKeys;
	SaveData sd(MAIL_TRUCK_ROUTE);
	sd.addValuesFrom(Route::getSaveData());
	sd.addBool(IS_DELIVERING, this->isDelivering);
	sd.addSizeT(NUM_STOPS, this->stops.size());
	// Add the stops
	for (auto it = this->stops.begin(); it != this->stops.end(); it++) {
		SaveData d = mailTruckRouteStopToSaveData(*it);
		d.addValue(INDEX, (it - this->stops.begin()));
		sd.addData(d);
	}
	return sd;
}
MailTruckRoute::MailTruckRoute(SaveData data) : Route(data) {
	using namespace SaveKeys;
	this->isDelivering = data.getBool(IS_DELIVERING);
	this->stops.resize(data.getSizeT(NUM_STOPS));
	for (SaveData d : data.getDatas()) {
		this->stops[d.getSizeT(INDEX)] = saveDataToMailTruckRouteStop(d);
	}
}
SaveData mailTruckRouteStopToSaveData(MailTruckRouteStop stop) {
	using namespace SaveKeys;
	SaveData stopData(MAIL_TRUCK_ROUTE_STOP);
	if (stop.target.has_value()) {
		sf::Vector2i target = stop.target.value();
		stopData.addBool(HAS_TARGET, true);
		stopData.addVector2i(TARGET, target);
	}
	else {
		stopData.addValue(HAS_TARGET, false);
	}
	return stopData;
}
MailTruckRouteStop saveDataToMailTruckRouteStop(SaveData data) {
	using namespace SaveKeys;
	MailTruckRouteStop stop;
	if (data.getBool(HAS_TARGET)) {
		stop.target = data.getVector2i(TARGET);
	}
	return stop;
}