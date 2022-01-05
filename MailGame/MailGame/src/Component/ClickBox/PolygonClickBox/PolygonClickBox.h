#pragma once
#include "Component/ClickBox/ClickBox.h"
#include "./Polygon.h"

/**
 * A clickbox in the shape of a convex polygon
 * May have different shapes based on rotation
 * (0,0) in the polygon is the entity's position
 */
class PolygonClickBox : public ClickBox {
public:
	// One polygon
	PolygonClickBox(Polygon p);
	virtual bool checkIfClicked(sf::Vector2f mouseCoords) override;
	virtual void renderClickBox(sf::RenderWindow* window) override;
private:
	Polygon poly;
};
