#pragma once
#include <SFML/System/Vector2.hpp>
#include <SFML/System/Vector3.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <utility>
#include "System/TransitType/TransitType.h"
#include "System/IsoRotation/IsoRotation.h"
#include "Constants.h"

class Game;

class TunnelEntrance {
public:
	TunnelEntrance(sf::Vector3i, IsoRotation);
	sf::Vector3i getPosition();
	IsoRotation getDirection();
private:
	sf::Vector3i position;
	IsoRotation direction;
};

class Tunnel {
public:
	// Constructor for a straight tunnel
	Tunnel(sf::Vector3i pointOne, sf::Vector3i pointTwo, TransitType type, Game* g);
	std::pair<TunnelEntrance, TunnelEntrance> getEntrances();
	std::vector<sf::Vector3f> getPoints();
	TransitType getType();
	bool canGetLock(sf::Vector2i point);
	void getLock(sf::Vector2i point);
	void releaseLock(sf::Vector2i point);
	void render(sf::RenderWindow* window);
	bool operator==(Tunnel other);
private:
	static id_t TUNNEL_ID;
	id_t id;
	// The points along the tunnel
	std::vector<sf::Vector3f> tunnelPoints;
	// The direction that the car/train/etc should enter the tunnel
	// startDirection is at the beginning of tunnelPoints, endDirection is at the end
	// Both are the direction to enter the tunnel at
	// i.e. one entrance is to enter the tile at tunnelPoints[0] from startDirection (i.e. the east)
	IsoRotation startDirection;
	IsoRotation endDirection;
	TransitType type;
	Game* game;
};