#pragma once
#include "Component/ClickBox/ClickBox.h"

// A polygon defined by a sequence of 2D points
// Check if the click is inside the polygon, assuming (0,0) is the entity's position for the points
class PolygonClickBox : public ClickBox {
public:
	PolygonClickBox(std::vector<sf::Vector2f> points);
	virtual bool checkIfClicked(sf::Vector2f mouseCoords) override;
	virtual void renderClickBox(sf::RenderWindow* window) override;
private:
	std::vector<sf::Vector2f> points;
};
