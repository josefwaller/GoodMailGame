#include "IsoSpriteRenderer.h"
#include "Entity/Entity.h"
#include "Game/Game.h"
#include "Component/Transform/Transform.h"
#include <SFML/Graphics/CircleShape.hpp>
#include "System/Utils/Utils.h"

IsoSpriteRenderer::IsoSpriteRenderer(sf::Sprite n, sf::Sprite e, sf::Sprite s, sf::Sprite w, sf::Vector3f off) {
	this->sprites.push_back(n);
	this->sprites.push_back(e);
	this->sprites.push_back(s);
	this->sprites.push_back(w);
	this->offset = off;
}

void IsoSpriteRenderer::render(sf::RenderWindow* window) {
	// Get sprite
	sf::Sprite spr = getSpriteToRender();
	// Create copy
	sf::Sprite shadowSpr(spr);
	// Set to black and render as a shadow on the ground
	shadowSpr.setColor(sf::Color(0, 0, 0, 100));
	sf::Vector3f pos = this->getEntity()->transform->getPosition();
	shadowSpr.setPosition(
		this->getEntity()->getGame()->worldToScreenPos(sf::Vector3f(pos.x, pos.y, 0.0f))
	);
	window->draw(shadowSpr);
	// Render original sprite
	window->draw(spr);
}
sf::Sprite IsoSpriteRenderer::getSpriteToRender() {
	// Get the rotation of the entity and the game combined
	Game* game = this->getEntity()->getGame();
	IsoRotation rot = this->getEntity()->transform->getRotation() + this->getEntity()->getGame()->getRotation();
	// Get the sprite with the proper rotation
	sf::Sprite toRender = this->sprites[rot.getRotation()];
	// Set position
	sf::Vector3f position = this->getEntity()->transform->getPosition();
	toRender.setPosition(game->worldToScreenPos(position + this->offset));
	return toRender;
}
