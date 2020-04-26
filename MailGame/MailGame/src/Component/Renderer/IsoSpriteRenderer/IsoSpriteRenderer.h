#pragma once
#include "Component/Renderer/Renderer.h"
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/System/Vector3.hpp>

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
		sf::Vector3f offset = { 0.0f, 0.0f, 0.0f });
	virtual void render(sf::RenderWindow* w) override;
protected:
	std::vector<sf::Sprite> sprites;
	// The offset with which to draw the sprite, in GAME coordinates, not screen coordinates
	sf::Vector3f offset;
	// Get the sprite, properly positioned and stuff
	// For use by subclasses so that they don't have to redo everything
	sf::Sprite getSpriteToRender();
};
