#pragma once
#include "Component/Component.h"
#include "System/IsoRotation/IsoRotation.h"
#include <SFML/System/Vector2.hpp>

class Transform: public Component {
public:
	Transform(sf::Vector2f initPos, IsoRotation rot = IsoRotation());
	sf::Vector2f getPosition();
	void setPosition(sf::Vector2f pos);

	// Check if the entity is on screen
	// Mainly used to check whether the UI should be drawn for ImGui
	bool isOnScreen();

	IsoRotation getRotation();
	void setRotation(IsoRotation rot);

	virtual std::optional<SaveData> getSaveData() override;
private:
	sf::Vector2f position;
	IsoRotation rotation;
};
