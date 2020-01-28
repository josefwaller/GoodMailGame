#pragma once
#include "Component/Renderer/Renderer.h"
#include <SFML/Graphics/Sprite.hpp>

/*
 * Renders different sprites depending on the entity's rotation/game rotation
 */
class IsoSpriteRenderer : public Renderer {
public:
	IsoSpriteRenderer(
		sf::Sprite north,
		sf::Sprite east,
		sf::Sprite south,
		sf::Sprite west,
		sf::Vector2f offset = { 0.0f, 0.0f });
	virtual void render(sf::RenderWindow* w) override;
protected:
	std::vector<sf::Sprite> sprites;
	// The offset with which to draw the sprite
	sf::Vector2f offset;
};
