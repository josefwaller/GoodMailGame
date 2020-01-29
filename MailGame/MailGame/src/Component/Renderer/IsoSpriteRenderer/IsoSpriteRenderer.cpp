#include "IsoSpriteRenderer.h"
#include "Entity/Entity.h"
#include "Game/Game.h"
#include "Component/Transform/Transform.h"
#include <SFML/Graphics/CircleShape.hpp>
#include "System/Utils/Utils.h"

IsoSpriteRenderer::IsoSpriteRenderer(sf::Sprite n, sf::Sprite e, sf::Sprite s, sf::Sprite w, sf::Vector2f off) {
	this->sprites.push_back(n);
	this->sprites.push_back(e);
	this->sprites.push_back(s);
	this->sprites.push_back(w);
	this->offset = off;
}

void IsoSpriteRenderer::render(sf::RenderWindow* window) {
	window->draw(getSpriteToRender());
}
sf::Sprite IsoSpriteRenderer::getSpriteToRender() {
	// Get the rotation of the entity and the game combined
	Game* game = this->getEntity()->getGame();
	IsoRotation rot = this->getEntity()->transform->getRotation() + this->getEntity()->getGame()->getRotation();
	// Get the sprite with the proper rotation
	sf::Sprite toRender = this->sprites[rot.getRotation()];
	// Set position
	sf::Vector2f position = this->getEntity()->transform->getPosition();
	toRender.setPosition(game->worldToScreenPos(position + this->offset));
	return toRender;
}
