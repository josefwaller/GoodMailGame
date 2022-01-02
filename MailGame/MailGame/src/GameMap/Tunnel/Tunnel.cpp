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

sf::Vector2i TunnelEntrance::getExitTile() {
	return Utils::toVector2i(this->position) + sf::Vector2i(this->direction.getReverse().getUnitVector());
}

Tunnel::Tunnel(sf::Vector3i pointOne, sf::Vector3i pointTwo, TransitType t, Game* game)
// Points are + Vector3f(0.5, 0.5, 0) since we want to center the positions on the x,y grid but keep z so that the car is on the ground
	: id(TUNNEL_ID++), type(t), game(game), isLocked(false),
	startDirection(IsoRotation::fromUnitVector(Utils::getUnitVector(sf::Vector3f(pointTwo - pointOne)))),
	endDirection(IsoRotation::fromUnitVector(Utils::getUnitVector(sf::Vector3f(pointOne - pointTwo)))) {
	sf::Vector3f first = sf::Vector3f(pointOne) + sf::Vector3f(0.5, 0.5, 0);
	sf::Vector3f last = sf::Vector3f(pointTwo) + sf::Vector3f(0.5, 0.5, 0);
	sf::Vector3f unit = Utils::getUnitVector(last - first);
	for (size_t i = 0; i < Utils::getVectorDistance(last, first); i++) {
		this->tunnelPoints.push_back(first);
		first += unit;
	}
	this->tunnelPoints.push_back(first);
	this->tunnelPoints.push_back(last);
}

Tunnel::Tunnel(SaveData data, Game* g) : game(g) {
	using namespace SaveKeys;
	this->id = data.getSizeT(ID);
	this->tunnelPoints = data.getVector3fVector(POINTS);
	this->startDirection = data.getIsoRotation(START);
	this->endDirection = data.getIsoRotation(END);
	this->isLocked = false;
	this->type = (TransitType)data.getSizeT(TYPE);
}

std::pair<TunnelEntrance, TunnelEntrance> Tunnel::getEntrances() {
	return {
		TunnelEntrance(sf::Vector3i(this->tunnelPoints.front()), this->startDirection),
		TunnelEntrance(sf::Vector3i(this->tunnelPoints.back()), this->endDirection)
	};
}

float Tunnel::getLength() {
	sf::Vector3f prevPoint = this->tunnelPoints.front();
	float len = 0.0f;
	for (auto p : this->tunnelPoints) {
		len += Utils::getVectorDistance(prevPoint, p);
		prevPoint = p;
	}
	return len;
}

TransitType Tunnel::getType() {
	return this->type;
}

bool Tunnel::canGetLock() {
	return !this->isLocked;
}

std::vector<sf::Vector3f> Tunnel::getPoints() {
	return this->tunnelPoints;
}

void Tunnel::getLock() {
	this->isLocked = true;
}

void Tunnel::releaseLock() {
	this->isLocked = false;
}

void Tunnel::render(sf::RenderWindow* window) {
	sf::Vector3f pOne(this->tunnelPoints.front() + this->startDirection.getReverse().getUnitVector3D() / 2.0f);
	sf::Vector3f pTwo(this->tunnelPoints.back() + this->endDirection.getReverse().getUnitVector3D() / 2.0f);
	sf::Color color = this->type == TransitType::Car ? sf::Color::Red : sf::Color::Cyan;
	sf::Color color2 = this->isLocked ? sf::Color::White : sf::Color::Black;
	sf::Vertex arr[2] = {
		sf::Vertex(this->game->worldToScreenPos(pOne), color),
		sf::Vertex(this->game->worldToScreenPos(pTwo), color2)
	};
	window->draw(arr, 2, sf::PrimitiveType::Lines);
}

bool Tunnel::operator==(Tunnel other) {
	return this->id == other.id;
}

float Tunnel::getDistance() {
	if (this->tunnelPoints.empty()) return 0.0f;

	float dist = 0.0f;
	for (auto it = this->tunnelPoints.begin(); it != this->tunnelPoints.end() - 1; it++) {
		dist += Utils::getVectorDistance(*it, *(it + 1));
	}
	return dist;
}

SaveData Tunnel::getSaveData() {
	using namespace SaveKeys;
	SaveData data(TUNNEL);
	data.addIsoRotation(START, this->startDirection);
	data.addIsoRotation(END, this->endDirection);
	data.addVector3fVector(POINTS, this->tunnelPoints);
	data.addSizeT(ID, this->id);
	data.addSizeT(TYPE, (size_t)this->type);
	data.addBool(IS_LOCKED, this->isLocked);
	return data;
}