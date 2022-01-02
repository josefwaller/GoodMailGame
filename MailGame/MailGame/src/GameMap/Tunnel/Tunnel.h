#pragma once
#include <SFML/System/Vector2.hpp>
#include <SFML/System/Vector3.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <utility>
#include "System/TransitType/TransitType.h"
#include "System/IsoRotation/IsoRotation.h"
#include "System/SaveData/SaveData.h"
#include "Constants.h"

class Game;

class TunnelEntrance {
public:
	TunnelEntrance(sf::Vector3i, IsoRotation);
	sf::Vector3i getPosition();
	// the direction facing into this tunnel entrance
	IsoRotation getDirection();
	// Get the tile that the vehicle would arrive at upon exiting the tunnel via this entrance
	sf::Vector2i getExitTile();
private:
	sf::Vector3i position;
	IsoRotation direction;
};

class Tunnel {
public:
	// Constructor for a straight tunnel
	Tunnel(sf::Vector3i pointOne, sf::Vector3i pointTwo, TransitType type, Game* g);
	Tunnel(SaveData data, Game* g);
	std::pair<TunnelEntrance, TunnelEntrance> getEntrances();
	TunnelEntrance getOtherEntrance(TunnelEntrance e);
	std::vector<sf::Vector3f> getPoints();
	TransitType getType();
	bool canGetLock();
	void getLock();
	void releaseLock();
	float getDistance();
	void render(sf::RenderWindow* window);
	bool operator==(Tunnel other);

	SaveData getSaveData();
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
	bool isLocked;
	Game* game;
};