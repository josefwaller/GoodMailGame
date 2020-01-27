#pragma once
#include <memory>
#include <SFML/System/Vector2.hpp>
#include "System/IsoRotation/IsoRotation.h"
#include "Routes/MailTruckRoute/MailTruckRoute.h"

class Entity;
class Game;

class EntityPresets {
public:
	static std::shared_ptr<Entity> building(Game* g, sf::Vector2f pos, IsoRotation rot);
	static std::shared_ptr<Entity> postOffice(Game* g, sf::Vector2f pos, IsoRotation rot);
	static std::shared_ptr<Entity> mailBox(Game* g, sf::Vector2f pos, IsoRotation rot);
	static std::shared_ptr<Entity> mailTruck(Game* g, sf::Vector2f pos, IsoRotation rot, MailTruckRoute route);
};
