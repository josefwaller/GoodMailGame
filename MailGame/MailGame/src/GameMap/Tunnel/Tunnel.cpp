#include "./Tunnel.h"
#include "System/Utils/Utils.h"
#include <SFML/System/Vector3.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include "Game/Game.h"

Tunnel::Tunnel(sf::Vector3i pointOne, sf::Vector3i pointTwo, TransitType t, Game* game) : points(std::make_tuple(pointOne, pointTwo)), type(t), game(game) {}

std::tuple<sf::Vector2i, sf::Vector2i> Tunnel::getEnds() {
	return std::make_tuple(
		Utils::toVector2i(std::get<0>(this->points)),
		Utils::toVector2i(std::get<1>(this->points))
	);
}

TransitType Tunnel::getType() {
	return this->type;
}

bool Tunnel::canGetLock(sf::Vector2i) {
	return true;
}

void Tunnel::getLock(sf::Vector2i) {
}

void Tunnel::releaseLock(sf::Vector2i) {
}

void Tunnel::render(sf::RenderWindow* window) {
	sf::Vector3f pOne(sf::Vector3f(std::get<0>(this->points)) + sf::Vector3f(0.5f, 0.5f, 0.5f));
	sf::Vector3f pTwo(sf::Vector3f(std::get<1>(this->points)) + sf::Vector3f(0.5f, 0.5f, 0.5f));
	sf::Color color = this->type == TransitType::Car ? sf::Color::Red : sf::Color::Cyan;
	sf::Vertex arr[2] = {
		sf::Vertex(this->game->worldToScreenPos(pOne), color),
		sf::Vertex(this->game->worldToScreenPos(pTwo), color)
	};
	window->draw(arr, 2, sf::PrimitiveType::Lines);
}