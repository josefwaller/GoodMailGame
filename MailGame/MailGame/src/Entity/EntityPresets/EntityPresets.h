#pragma once
#include <memory>
#include <SFML/System/Vector2.hpp>
#include "System/IsoRotation/IsoRotation.h"
#include "Routes/MailTruckRoute/MailTruckRoute.h"
#include "Routes/CargoTruckRoute/CargoTruckRoute.h"

class Entity;
class Game;

class EntityPresets {
public:
	static std::shared_ptr<Entity> residence(Game* g, sf::Vector2f pos, IsoRotation rot);
	static std::shared_ptr<Entity> postOffice(Game* g, sf::Vector2f pos, IsoRotation rot);
	static std::shared_ptr<Entity> mailBox(Game* g, sf::Vector2f pos, IsoRotation rot);
	static std::shared_ptr<Entity> mailTruck(
		Game* g, sf::Vector2f pos, IsoRotation rot, MailTruckRoute route, std::weak_ptr<Entity> office
	);
	static std::shared_ptr<Entity> cargoTruckDepot(Game* g, sf::Vector2f pos, IsoRotation rot);
	static std::shared_ptr<Entity> cargoTruck(
		Game* g, sf::Vector2f pos, IsoRotation rot, CargoTruckRoute route, std::weak_ptr<Entity> office
	);
	static std::shared_ptr<Entity> trainStation(Game* g, sf::Vector2f pos, IsoRotation rot);
};
