#pragma once
#include <vector>
#include <SFML/System/Vector2.hpp>
/**
 * A convex polygon, used for click detection
 */
class Polygon {
public:
	// Initialize with a list of points, in order
	// Point i is connected to points i - 1 and i + 1
	Polygon(std::vector<sf::Vector2f> points);
	// Get the points
	std::vector<sf::Vector2f> getPoints();
private:
	// The points
	std::vector<sf::Vector2f> points;
};
