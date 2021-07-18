#pragma once
#include <SFML/System/Vector2.hpp>
#include <SFML/System/Vector3.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <tuple>
#include "System/TransitType/TransitType.h"

class Game;

class Tunnel {
public:
	Tunnel(sf::Vector3i pointOne, sf::Vector3i pointTwo, TransitType type, Game* g);
	std::tuple<sf::Vector3i, sf::Vector3i> getEnds();
	TransitType getType();
	bool canGetLock(sf::Vector2i point);
	void getLock(sf::Vector2i point);
	void releaseLock(sf::Vector2i point);
	void render(sf::RenderWindow* window);
	bool operator==(Tunnel other);
private:
	std::tuple<sf::Vector3i, sf::Vector3i> points;
	TransitType type;
	Game* game;
};