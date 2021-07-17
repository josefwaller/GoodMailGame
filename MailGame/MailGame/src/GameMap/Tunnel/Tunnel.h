#pragma once
#include <SFML/System/Vector2.hpp>
#include <SFML/System/Vector3.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <tuple>

class Game;

class Tunnel {
public:
	Tunnel(sf::Vector3i pointOne, sf::Vector3i pointTwo, Game* g);
	std::tuple<sf::Vector2i, sf::Vector2i> getEnds();
	bool canGetLock(sf::Vector2i point);
	void getLock(sf::Vector2i point);
	void releaseLock(sf::Vector2i point);
	void render(sf::RenderWindow* window);
private:
	std::tuple<sf::Vector3i, sf::Vector3i> points;
	Game* game;
};