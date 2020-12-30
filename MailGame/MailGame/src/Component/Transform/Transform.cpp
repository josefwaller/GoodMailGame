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
	SaveData d(componentTypeToStr(ComponentType::Transform));
	d.addValue("x", std::to_string(this->position.x));
	d.addValue("y", std::to_string(this->position.y));
	d.addValue("z", std::to_string(this->position.z));
	d.addValue("rot", std::to_string(this->rotation.getRotation()));
	return d;
}
void Transform::fromSaveData(SaveData data) {
	this->position.x = std::stof(data.getValue("x"));
	this->position.y = std::stof(data.getValue("y"));
	this->position.z = std::stof(data.getValue("z"));
	this->rotation = IsoRotation(std::stoi(data.getValue("rot")));
}