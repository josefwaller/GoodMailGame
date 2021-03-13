#include "RoutePoint.h"

RoutePoint::RoutePoint(sf::Vector3f pos, gtime_t time, float s, float a, float distance) : pos(pos), expectedTime(time), distance(distance), speedAtPoint(s), accelerationAtPoint(a) {};
RoutePoint saveDataToRoutePoint(SaveData d) {
	using namespace SaveKeys;
	return RoutePoint(d.getVector3f(POSITION),
		d.getGTimeT(EXPECTED_TIME),
		0.0f,
		0.0f
	);
}
SaveData routePointToSaveData(RoutePoint p) {
	using namespace SaveKeys;
	SaveData d(ROUTE_POINT);
	d.addVector3f(POSITION, p.pos);
	d.addGTimeT(EXPECTED_TIME, p.expectedTime);
	return d;
}