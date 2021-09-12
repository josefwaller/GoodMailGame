#include "./RailLock.h"
#include "System/IsoRotation/IsoRotation.h"

RailLock::RailLock(Tunnel t, float d) : isTunnel(true), value(t), distance(d) {}
RailLock::RailLock(sf::Vector2i tile, Railway rail, float d) : isTunnel(false), value(std::pair<sf::Vector2i, Railway>(tile, rail)), distance(d) {}

bool RailLock::getIsTunnel() {
	return this->isTunnel;
}

Tunnel RailLock::getLockedTunnel() {
	return std::get<Tunnel>(this->value);
}

std::pair<sf::Vector2i, Railway> RailLock::getLockedTile() {
	return std::get<std::pair<sf::Vector2i, Railway>>(this->value);
}

float RailLock::getDistance() {
	return this->distance;
}