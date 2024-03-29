#pragma once
#include "Component/Ai/Ai.h"
#include <SFML/System/Vector2.hpp>
#include "Routes/MailTruckRoute/MailTruckRoute.h"
#include <vector>

class GameMap;

class PickupDeliveryAi : public Ai {
public:
	PickupDeliveryAi(MailTruckRoute route, std::weak_ptr<Entity> office);
	static const float SPEED;
	virtual void renderUi() override;
	virtual std::optional<SaveData> getSaveData() override;
	virtual std::vector<VehicleControllerStop> getStops() override;
	void fromSaveData(SaveData data);
private:
	// Flag for if the truck has picked up the mail from the office yet
	bool hasPickedUpMail;
	// The route the truck is doing
	MailTruckRoute route;
	// The office
	std::weak_ptr<Entity> office;
	// Overridden methods
	virtual void onArriveAtTile(sf::Vector2i point) override;
	virtual void onArriveAtDest() override;
	// Drop off any mail it has for the tiles around the given position
	void dropOffMail(sf::Vector2i pos);
	// Pick up the mail along the truck's route from the post office that spawned it
	void pickupMailFromOffice();
};
