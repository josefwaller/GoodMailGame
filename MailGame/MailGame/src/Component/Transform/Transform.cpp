#include "Transform.h"

Transform::Transform(std::weak_ptr<Entity> e, sf::Vector2f pos) : Component(e) {
	this->position = pos;
}
sf::Vector2f Transform::getPosition() {
	return this->position;
}

void Transform::setPosition(sf::Vector2f pos) {
	this->position = pos;
}