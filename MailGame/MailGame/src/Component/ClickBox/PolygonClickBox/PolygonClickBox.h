#pragma once
#include "Component/ClickBox/ClickBox.h"
#include "./Polygon.h"
#include <SFML/System/Vector3.hpp>

/**
 * A clickbox in the shape of a convex polygon
 * (0,0) in the polygon is the entity's position
 */
class PolygonClickBox : public ClickBox {
public:
	// Allow for an offset before transforming from world to screen coords
	// Useful for buildings to cneter their hitboxes
	PolygonClickBox(Polygon p, sf::Vector3f worldOffset = sf::Vector3f(0, 0, 0));
	virtual bool checkIfClicked(sf::Vector2f mouseCoords) override;
	virtual void renderClickBox(sf::RenderWindow* window) override;
private:
	Polygon poly;
	sf::Vector3f worldOffset;
	sf::Vector2f getOffset();
};
