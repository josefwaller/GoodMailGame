#include "Transform.h"
#include "Entity/Entity.h"
#include "Game/Game.h"
#include "Component/ComponentType/ComponentType.h"
#include "System/SaveData/SaveData.h"

Transform::Transform(sf::Vector3f pos, IsoRotation rot) {
	this->position = pos;
	this->rotation = rot;
}
sf::Vector3f Transform::getPosition() {
	return this->position;
}

bool Transform::isOnScreen() {
	sf::Vector2f screenPos = this->getEntity()->getGame()->worldToScreenPos(this->getPosition());
	sf::FloatRect viewport = this->getEntity()->getGame()->getViewport();

	return viewport.contains(screenPos);
}

void Transform::setPosition(sf::Vector3f pos) {
	this->position = pos;
}

IsoRotation Transform::getRotation() {
	return this->rotation;
}

void Transform::setRotation(IsoRotation rot) {
	this->rotation = rot;
}

std::optional<SaveData> Transform::getSaveData() {
	using namespace SaveKeys;
	SaveData d(componentTypeToStr(ComponentType::Transform));
	d.addVector3f(POSITION, this->position);
	d.addIsoRotation(ROTATION, this->rotation);
	return d;
}
void Transform::fromSaveData(SaveData data) {
	using namespace SaveKeys;
	this->position = data.getVector3f(POSITION);
	this->rotation = data.getIsoRotation(ROTATION);
}