#include "MailTruckRoute.h"
#include "System/SaveData/SaveData.h"

// Initialize id to 0
size_t MailTruckRoute::ROUTE_ID = 0;

SaveData mailTruckRouteToSaveData(MailTruckRoute route) {
	SaveData sd("MailTruckRoute");
	return sd;
}
