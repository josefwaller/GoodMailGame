#pragma once
#include "Component/Controller/Controller.h"
#include <SFML/System/Vector2.hpp>
#include "Routes/MailTruckRoute/MailTruckRoute.h"
#include <vector>

class MailTruckController : public Controller {
public:
	MailTruckController(MailTruckRoute route);
	static const float SPEED;
	virtual void update(float delta) override;
private:
	// The points the truck is currently going through on its route
	std::vector<sf::Vector2i> points;
	size_t pointIndex;
	// The route the truck is doing
	MailTruckRoute route;
	size_t stopIndex;
	// The last point that the truck checked to drop off letters
	// Used to avoid checking every frame
	sf::Vector2i lastPoint;
	// Set the points for the next stop
	void pathfindToNextStop();
	// Drop off any mail it has for the tiles around the given position
	void dropOffMail(sf::Vector2i pos);
};
