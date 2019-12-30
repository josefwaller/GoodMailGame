#include "Transform.h"

Transform::Transform(sf::Vector2f pos) {
	this->position = pos;
}
sf::Vector2f Transform::getPosition() {
	return this->position;
}

void Transform::setPosition(sf::Vector2f pos) {
	this->position = pos;
}