#include "RoutePoint.h"

RoutePoint saveDataToRoutePoint(SaveData d) {
	return RoutePoint(
		sf::Vector3f(
			d.getValuef("x"),
			d.getValuef("y"),
			d.getValuef("z")
		),
		std::stoull(d.getValue("expectedTime"))
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