#include "RoutePoint.h"

RoutePoint::RoutePoint(sf::Vector3f pos, gtime_t time, float s, float a, float distance) : pos(pos), expectedTime(time), distance(distance), speedAtPoint(s), accelerationAtPoint(a) {};
RoutePoint saveDataToRoutePoint(SaveData d) {
	return RoutePoint(
		sf::Vector3f(
			d.getValuef("x"),
			d.getValuef("y"),
			d.getValuef("z")
		),
		std::stoull(d.getValue("expectedTime")),
		0.0f,
		0.0f
	);
}
SaveData routePointToSaveData(RoutePoint p) {
	SaveData d("RoutePoint");
	d.addValue("x", p.pos.x);
	d.addValue("y", p.pos.y);
	d.addValue("z", p.pos.z);
	d.addValue("expectedTime", p.expectedTime);
	return d;
}