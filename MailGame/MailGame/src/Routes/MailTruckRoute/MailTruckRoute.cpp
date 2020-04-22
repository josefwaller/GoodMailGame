#include "MailTruckRoute.h"
#include "System/SaveData/SaveData.h"

// Initialize id to 0
size_t MailTruckRoute::ROUTE_ID = 0;

SaveData mailTruckRouteToSaveData(MailTruckRoute route) {
	SaveData sd("MailTruckRoute");
	sd.addValue("isDelivering", route.isDelivering);
	sd.addValue("departTime", route.departTime);
	sd.addValue("id", route.id);
	// Add the stops
	for (MailTruckRouteStop stop : route.stops) {
		SaveData stopData("MailTruckRouteStop");
		if (stop.target.has_value()) {
			sf::Vector2i target = stop.target.value();
			stopData.addValue("hasValue", true);
			stopData.addValue("x", target.x);
			stopData.addValue("y", target.y);
		}
		else {
			stopData.addValue("hasValue", false);
		}
		sd.addData(stopData);
	}
	return sd;
}
