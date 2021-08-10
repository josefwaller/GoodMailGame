#include "./Tunnel.h"
#include "System/Utils/Utils.h"
#include <SFML/System/Vector3.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include "Game/Game.h"

id_t Tunnel::TUNNEL_ID = 0;

TunnelEntrance::TunnelEntrance(sf::Vector3i t, IsoRotation d) : position(t), direction(d) {};

sf::Vector3i TunnelEntrance::getPosition() {
	return this->position;
}

IsoRotation TunnelEntrance::getDirection() {
	return this->direction;
}
Tunnel::Tunnel(sf::Vector3i pointOne, sf::Vector3i pointTwo, TransitType t, Game* game)
// Points are + Vector3f(0.5, 0.5, 0) since we want to center the positions on the x,y grid but keep z so that the car is on the ground
	: id(TUNNEL_ID++), tunnelPoints({ sf::Vector3f(pointOne) + sf::Vector3f(0.5, 0.5, 0), sf::Vector3f(pointTwo) + sf::Vector3f(0.5, 0.5, 0) }), type(t), game(game),
	startDirection(IsoRotation::fromUnitVector(Utils::getUnitVector(sf::Vector3f(pointTwo - pointOne)))),
	endDirection(IsoRotation::fromUnitVector(Utils::getUnitVector(sf::Vector3f(pointOne - pointTwo)))) {}

std::pair<TunnelEntrance, TunnelEntrance> Tunnel::getEntrances() {
	return {
		TunnelEntrance(sf::Vector3i(this->tunnelPoints.front()), this->startDirection),
		TunnelEntrance(sf::Vector3i(this->tunnelPoints.back()), this->endDirection)
	};
}

TransitType Tunnel::getType() {
	return this->type;
}

bool Tunnel::canGetLock(sf::Vector2i) {
	return true;
}

std::vector<sf::Vector3f> Tunnel::getPoints() {
	return this->tunnelPoints;
}

void Tunnel::getLock(sf::Vector2i) {
}

void Tunnel::releaseLock(sf::Vector2i) {
}

void Tunnel::render(sf::RenderWindow* window) {
	sf::Vector3f pOne(sf::Vector3f(this->tunnelPoints.front()) + sf::Vector3f(0.5f, 0.5f, 0.5f));
	sf::Vector3f pTwo(sf::Vector3f(this->tunnelPoints.back()) + sf::Vector3f(0.5f, 0.5f, 0.5f));
	sf::Color color = this->type == TransitType::Car ? sf::Color::Red : sf::Color::Cyan;
	sf::Vertex arr[2] = {
		sf::Vertex(this->game->worldToScreenPos(pOne), color),
		sf::Vertex(this->game->worldToScreenPos(pTwo), color)
	};
	window->draw(arr, 2, sf::PrimitiveType::Lines);
}

bool Tunnel::operator==(Tunnel other) {
	return this->id == other.id;
}