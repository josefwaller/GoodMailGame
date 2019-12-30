#pragma once
#include "Component/Component.h"
#include <SFML/System/Vector2.hpp>

class Transform: public Component {
public:
	Transform(std::weak_ptr<Entity> e, sf::Vector2f initPos);
	sf::Vector2f getPosition();
	void setPosition(sf::Vector2f pos);

private:
	sf::Vector2f position;
};
