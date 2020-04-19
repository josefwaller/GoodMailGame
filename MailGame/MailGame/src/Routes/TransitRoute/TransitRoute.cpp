#include "TransitRoute.h"
#include "System/SaveData/SaveData.h"

unsigned long long TransitRouteStop::STOP_ID = 0;
unsigned long long TransitRoute::ROUTE_ID = 0;

SaveData transitRouteToSaveData(TransitRoute route) {
	return SaveData("TransitRoute");
}

SaveData transitRouteStopToSaveData(TransitRouteStop stop) {
	return SaveData("TransitRouteStop");
}
