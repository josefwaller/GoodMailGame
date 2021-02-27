#include "SpeedPoint.h"

SpeedPoint::SpeedPoint(sf::Vector3f p) : pos(p) {};
SpeedPoint::SpeedPoint(sf::Vector3f p, float s) : pos(p), speed({ s }) {};
sf::Vector3f SpeedPoint::getPos() { return this->pos; };
std::optional<float> SpeedPoint::getSpeed() { return this->speed; };
void SpeedPoint::setSpeed(float s) {
	this->speed = s;
}