#pragma once
#include "Component/Component.h"
#include "System/IsoRotation/IsoRotation.h"
#include <SFML/System/Vector3.hpp>

class Transform : public Component {
public:
	Transform(sf::Vector3f initPos, IsoRotation rot = IsoRotation());
	sf::Vector3f getPosition();
	void setPosition(sf::Vector3f pos);

	// Check if the entity is on screen
	// Mainly used to check whether the UI should be drawn for ImGui
	bool isOnScreen();

	IsoRotation getRotation();
	void setRotation(IsoRotation rot);

	virtual std::optional<SaveData> getSaveData() override;
	void fromSaveData(SaveData data) override;
private:
	sf::Vector3f position;
	IsoRotation rotation;
};
