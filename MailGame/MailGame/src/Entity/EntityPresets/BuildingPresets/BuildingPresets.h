#pragma once
#include <memory>
#include <SFML/System/Vector3.hpp>
#include "System/IsoRotation/IsoRotation.h"
#include <SFML/Graphics/Rect.hpp>

class Entity;
class Game;

class BuildingPresets {
public:
	static std::shared_ptr<Entity> residence(Game* g, sf::Vector3f pos, IsoRotation rot);
	static std::shared_ptr<Entity> postOffice(Game* g, sf::Vector3f pos, IsoRotation rot);
	static std::shared_ptr<Entity> mailBox(Game* g, sf::Vector3f pos, IsoRotation rot);
	static std::shared_ptr<Entity> cargoTruckDepot(Game* g, sf::Vector3f pos, IsoRotation rot);
	static std::shared_ptr<Entity> trainStation(Game* g, sf::Vector3f pos, IsoRotation rot);
	static std::shared_ptr<Entity> airport(Game* g, sf::Vector3f pos, IsoRotation rot);
private:
	// Create a road leading into the building so that it can be accessed by cars
	// Road is always going in from the front
	static void addRoadForTransitBuilding(Game* g, sf::Vector3i pos, IsoRotation rot);
	// Set the building value in the tiles to the building given
	// Set for all tiles within (x, y) to (x + w, y + h)
	static void setTilesToBuilding(Game* g, std::weak_ptr<Entity> building, sf::IntRect rect);
};
