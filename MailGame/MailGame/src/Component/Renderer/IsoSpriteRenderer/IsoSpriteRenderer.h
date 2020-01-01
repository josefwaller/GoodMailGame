#pragma once
#include "Component/Renderer/Renderer.h"
#include <SFML/Graphics/Sprite.hpp>

/*
 * Renders different sprites depending on the entity's rotation/game rotation
 */
class IsoSpriteRenderer : public Renderer {
public:
	IsoSpriteRenderer(sf::Sprite north, sf::Sprite east, sf::Sprite south, sf::Sprite west);
	virtual void render(sf::RenderWindow* w) override;
private:
	std::vector<sf::Sprite> sprites;
};
