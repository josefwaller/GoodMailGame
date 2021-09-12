#pragma once
#include "GameMap/Tunnel/Tunnel.h"
#include "GameMap/Tile/Railway/Railway.h"
#include <SFML/System/Vector2.hpp>
#include <variant>

/**
 * Utility class for holding the lock a train has, i.e. a tunnel, tile or bridge
 */
class RailLock {
private:
	bool isTunnel;
	std::variant<Tunnel, std::pair<sf::Vector2i, Railway>> value;
	float distance;
public:
	RailLock(Tunnel t, float distance);
	RailLock(sf::Vector2i tile, Railway railway, float distance);
	bool getIsTunnel();
	Tunnel getLockedTunnel();
	std::pair<sf::Vector2i, Railway> getLockedTile();
	float getDistance();
};
