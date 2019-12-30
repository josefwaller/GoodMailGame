#pragma once
#include "Component\Component.h"
#include <SFML/Graphics/RenderWindow.hpp>

class Renderer : public Component {
public:
	Renderer(std::weak_ptr<Entity>);
	virtual void render(sf::RenderWindow* w) = 0;
};
