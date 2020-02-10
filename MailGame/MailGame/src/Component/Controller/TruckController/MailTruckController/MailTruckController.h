#pragma once
#include "Component/Controller/TruckController/TruckController.h"
#include <SFML/System/Vector2.hpp>
#include "Routes/MailTruckRoute/MailTruckRoute.h"
#include <vector>

class GameMap;

class MailTruckController : public TruckController {
public:
	MailTruckController(MailTruckRoute route, std::weak_ptr<Entity> office);
	static const float SPEED;
	virtual void update(float delta) override;
private:
	// The route the truck is doing
	MailTruckRoute route;
	size_t stopIndex;
	// Whether the truck is heading back to the post office
	bool isGoingToOffice;
	// The office
	std::weak_ptr<Entity> office;
	// Get Speed, see TruckController
	virtual float getSpeed() override;
	virtual void onArriveAtTile(sf::Vector2f point) override;
	virtual void onArriveAtDest() override;
	// Drop off any mail it has for the tiles around the given position
	void dropOffMail(sf::Vector2i pos);
};
