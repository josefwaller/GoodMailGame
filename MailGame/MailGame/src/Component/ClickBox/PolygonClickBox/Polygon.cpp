#include "./Polygon.h"

Polygon::Polygon(std::vector<sf::Vector2f> p) : points(p) {}

std::vector<sf::Vector2f> Polygon::getPoints() { return this->points; }
