#pragma once
#include <memory>
#include <SFML/System/Vector2.hpp>
#include "System/IsoRotation/IsoRotation.h"

class Entity;
class Game;

class BuildingPresets {
public:
	static std::shared_ptr<Entity> residence(Game* g, sf::Vector2f pos, IsoRotation rot);
	static std::shared_ptr<Entity> postOffice(Game* g, sf::Vector2f pos, IsoRotation rot);
	static std::shared_ptr<Entity> mailBox(Game* g, sf::Vector2f pos, IsoRotation rot);
	static std::shared_ptr<Entity> cargoTruckDepot(Game* g, sf::Vector2f pos, IsoRotation rot);
	static std::shared_ptr<Entity> trainStation(Game* g, sf::Vector2f pos, IsoRotation rot);
private:
	// Create a road leading into the building so that it can be accessed by cars
	// Road is always going in from the front
	static void addRoadForTransitBuilding(Game* g, sf::Vector2i pos, IsoRotation rot);
};
