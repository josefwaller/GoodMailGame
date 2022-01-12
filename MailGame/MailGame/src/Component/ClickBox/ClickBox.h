#pragma once
#include "Component/Component.h"
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

/* A Hitbox used to detect if the entity is being clicked */
class ClickBox : public Component {
public:
	virtual bool checkIfClicked(sf::Vector2f mouseCoords) = 0;
	virtual void renderClickBox(sf::RenderWindow* window) = 0;
};
