#pragma once
#include "Component/Component.h"
#include <SFML/System/Vector2.hpp>
#ifdef _DEBUG
#include <SFML/Graphics/RenderWindow.hpp>
#endif

/* A Hitbox used to detect if the entity is being clicked */
class ClickBox : public Component {
public:
	virtual bool checkIfClicked(sf::Vector2f mouseCoords) = 0;
#ifdef _DEBUG
	virtual void renderClickBox(sf::RenderWindow* window) = 0;
#endif
};
