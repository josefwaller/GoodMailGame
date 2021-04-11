#pragma once
#include <memory>
#include <SFML/System/Vector3.hpp>
#include "System/IsoRotation/IsoRotation.h"
#include "Routes/MailTruckRoute/MailTruckRoute.h"
#include "Routes/TransitRoute/TransitRoute.h"

class Game;
class Entity;
#define entity_ptr static std::shared_ptr<Entity>

class VehiclePresets {
public:
	entity_ptr mailTruck(
		Game* g, sf::Vector3f pos, IsoRotation rot, MailTruckRoute route, std::weak_ptr<Entity> office, gtime_t spawnTime
	);
	entity_ptr cargoTruck(
		Game* g, sf::Vector3f pos, IsoRotation rot, TransitRoute route, std::weak_ptr<Entity> office, gtime_t spawnTime
	);
	entity_ptr train(
		Game* g, sf::Vector3f pos, IsoRotation rot, TransitRoute route, std::weak_ptr<Entity> depot, gtime_t spawnTime
	);
	entity_ptr plane(
		Game* g, sf::Vector3f pos, IsoRotation rot, TransitRoute route, std::weak_ptr<Entity> depot, gtime_t spawnTime
	);
	entity_ptr trainCar(Game* g, sf::Vector3f pos, IsoRotation rot, IsoSprite sprites);
};

#undef entity_ptr
