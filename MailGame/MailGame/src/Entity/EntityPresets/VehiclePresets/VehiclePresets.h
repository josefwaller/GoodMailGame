#pragma once
#include <memory>
#include <SFML/System/Vector2.hpp>
#include "System/IsoRotation/IsoRotation.h"
#include "Routes/MailTruckRoute/MailTruckRoute.h"
#include "Routes/TransitRoute/TransitRoute.h"

class Game;
class Entity;
#define entity_ptr static std::shared_ptr<Entity>

class VehiclePresets {
public:
	entity_ptr mailTruck(
		Game* g, sf::Vector2f pos, IsoRotation rot, MailTruckRoute route, std::weak_ptr<Entity> office
	);
	entity_ptr cargoTruck(
		Game* g, sf::Vector2f pos, IsoRotation rot, TransitRoute route, std::weak_ptr<Entity> office
	);
	entity_ptr train(
		Game* g, sf::Vector2f pos, IsoRotation rot, TransitRoute route, std::weak_ptr<Entity> depot
	);
};

#undef entity_ptr
