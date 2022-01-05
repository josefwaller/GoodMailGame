#pragma once
#include <vector>
#include <SFML/System/Vector3.hpp>
/**
 * A convex polygon in 3D space, used for click detection
 */
class Polygon {
public:
	// Initialize with a list of points, in order
	// Point i is connected to points i - 1 and i + 1
	Polygon(std::vector<sf::Vector3f> points);
	// Get the points
	std::vector<sf::Vector3f> getPoints();
private:
	// The points
	std::vector<sf::Vector3f> points;
};
