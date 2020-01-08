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
	sf::Vector2i screenSize = this->getEntity()->getGame()->getWindowSize();

	bool xIsOnScreen = screenPos.x >= 0 && screenPos.x < screenSize.x;
	bool yIsOnScreen = screenPos.y >= 0 && screenPos.y < screenSize.y;

	return xIsOnScreen && yIsOnScreen;
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
