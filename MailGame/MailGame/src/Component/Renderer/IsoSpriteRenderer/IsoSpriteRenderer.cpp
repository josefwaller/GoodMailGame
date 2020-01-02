#include "IsoSpriteRenderer.h"
#include "Entity/Entity.h"
#include "Game/Game.h"
#include "Component/Transform/Transform.h"
#include <SFML/Graphics/CircleShape.hpp>

IsoSpriteRenderer::IsoSpriteRenderer(sf::Sprite n, sf::Sprite e, sf::Sprite s, sf::Sprite w) {
	this->sprites.push_back(n);
	this->sprites.push_back(e);
	this->sprites.push_back(s);
	this->sprites.push_back(w);
}

void IsoSpriteRenderer::render(sf::RenderWindow* window) {
	sf::Vector2f position = this->getEntity()->transform->getPosition();
	IsoRotation rot = this->getEntity()->transform->getRotation() + this->getEntity()->getGame()->getRotation();
	sf::Sprite toRender = this->sprites[rot.getRotation()];
	// Set sprite origin to bottom-center
	Game* game = this->getEntity()->getGame();
	toRender.setOrigin(toRender.getLocalBounds().width / 2, toRender.getLocalBounds().height - 32);
	toRender.setPosition(game->worldToScreenPos(position));
	window->draw(toRender);
}
