#pragma once
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/System/Vector3.hpp>
#include <SFML/Graphics/Color.hpp>
#include "Constants.h"
#include "Routes/RoutePoint.h"
#include "System/SpeedPoint/SpeedPoint.h"
#include "VehicleModel/VehicleModel.h"

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
	// Get unit vector
	static sf::Vector3f getUnitVector(sf::Vector3f vector);
	static sf::Vector2i getUnitVector(sf::Vector2i v);
	// Get the color to represent a certain time of day
	static sf::Color getTimeColor(gtime_t time);

	// Get the plane that goes through the three points
	// Returned in the format ax + by + cz = d
	static std::vector<float> getPlaneThroughPoints(sf::Vector3f A, sf::Vector3f B, sf::Vector3f C);
	// Get the intersection of a vector and a plane
	static sf::Vector3f getVectorPlaneIntersection(sf::Vector3f vectorPos, sf::Vector3f vectorDirection, std::vector<float> plane);
	static std::vector<RoutePoint> toRoutePointVector(std::vector<sf::Vector3f> points, gtime_t time, float speed);
	// Convert a list of SpeedPoints to a list of RoutePoints
	static std::vector<RoutePoint> speedPointVectorToRoutePointVector(std::vector<SpeedPoint> points, gtime_t departTime, VehicleModel model, float startingSpeed = 0.0f);
	// Convert vector2f to 2i
	static sf::Vector2i toVector2i(sf::Vector2f p);
	static sf::Vector2i toVector2i(sf::Vector3f p);
	static sf::Vector2i toVector2i(sf::Vector3i p);
	// Convert to vector2f
	static sf::Vector2f toVector2f(sf::Vector3f p);
	// Convert to vector 3f
	static sf::Vector3f toVector3f(sf::Vector2i p);
	static sf::Vector3f toVector3f(sf::Vector2f p);
	// Convert to vector 3i
	static sf::Vector3i toVector3i(sf::Vector2i p);
};
