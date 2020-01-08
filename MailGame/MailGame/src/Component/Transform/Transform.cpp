#include "Transform.h"
#include "Entity/Entity.h"
#include "Game/Game.h"

Transform::Transform(sf::Vector2f pos, IsoRotation rot) {
	this->position = pos;
	this->rotation = rot;
}
sf::Vector2f Transform::getPosition() {
	return this->position;
}

bool Transform::isOnScreen() {
	sf::Vector2f screenPos = this->getEntity()->getGame()->worldToScreenPos(this->getPosition());
	sf::FloatRect viewport = this->getEntity()->getGame()->getViewport();

	return viewport.contains(screenPos);
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
