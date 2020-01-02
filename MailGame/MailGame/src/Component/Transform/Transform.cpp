#include "Transform.h"

Transform::Transform(sf::Vector2f pos, IsoRotation rot) {
	this->position = pos;
	this->rotation = rot;
}
sf::Vector2f Transform::getPosition() {
	return this->position;
}

void Transform::setPosition(sf::Vector2f pos) {
	this->position = pos;
}

IsoRotation Transform::getRotation() {
	return this->rotation;
}

void Transform::setRotation(IsoRotation rot) {
	this->rotation = rot;
}
