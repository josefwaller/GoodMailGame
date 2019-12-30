#pragma once
#include "Component\Component.h"
#include <SFML/Graphics/RenderWindow.hpp>

class Renderer : public Component {
public:
	virtual void render(sf::RenderWindow* w) = 0;
};
