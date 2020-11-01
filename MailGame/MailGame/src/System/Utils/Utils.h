#pragma once
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/System/Vector3.hpp>
#include <SFML/Graphics/Color.hpp>
#include "Constants.h"
#include "Routes/RoutePoint.h"
#include "System/SpeedPoint/SpeedPoint.h"

// Misc Utils that I couldn't think to put anywhere else
// Mostly temporary things that shouldn't make it into the final product
class Utils {
public:
	// Setup a sprite to be drawn as a building, since the spritesheet I'm using right now
	// has some extra ground for buildings
	// This is pretty messy, but hopefully will not make it into the final version
	static sf::Sprite setupBuildingSprite(sf::Sprite spr, bool isBuilding = true);
	// Get distance between vector3fs
	static float getVectorDistance(sf::Vector3f one, sf::Vector3f two);
	// Get the color to represent a certain time of day
	static sf::Color getTimeColor(gtime_t time);

	static std::vector<RoutePoint> toRoutePointVector(std::vector<sf::Vector3f> points, gtime_t time, float speed);
	// Convert a list of SpeedPoints to a list of RoutePoints
	static std::vector<RoutePoint> speedPointVectorToRoutePointVector(std::vector<SpeedPoint> points, gtime_t departTime, float defaultSpeed);
};
