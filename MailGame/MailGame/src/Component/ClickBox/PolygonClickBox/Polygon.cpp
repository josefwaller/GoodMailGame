#include "./Polygon.h"

Polygon::Polygon(std::vector<sf::Vector3f> p) : points(p) {}

std::vector<sf::Vector3f> Polygon::getPoints() { return this->points; }