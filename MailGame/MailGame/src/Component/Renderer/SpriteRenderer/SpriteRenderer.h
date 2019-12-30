#pragma once
#include "Component/Renderer/Renderer.h"
#include <SFML/Graphics/Sprite.hpp>

class SpriteRenderer : public Renderer {
public:
	SpriteRenderer(sf::Sprite spr);
	virtual void render(sf::RenderWindow* window) override;
private:
	sf::Sprite sprite;
};
