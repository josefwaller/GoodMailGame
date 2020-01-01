#pragma once
#include "Component/Component.h"
#include "System/IsoRotation/IsoRotation.h"
#include <SFML/System/Vector2.hpp>

class Transform: public Component {
public:
	Transform(sf::Vector2f initPos, IsoRotation rot = IsoRotation());
	sf::Vector2f getPosition();
	void setPosition(sf::Vector2f pos);

	IsoRotation getRotation();
	void setRotation(IsoRotation rot);
private:
	sf::Vector2f position;
	IsoRotation rotation;
};
