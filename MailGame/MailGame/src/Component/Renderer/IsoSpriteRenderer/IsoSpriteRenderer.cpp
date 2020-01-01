#include "IsoSpriteRenderer.h"
#include "Entity/Entity.h"
#include "Game/Game.h"
#include "Component/Transform/Transform.h"

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
	Game* game = this->getEntity()->getGame();
	toRender.setPosition(game->worldToScreenPos(position));
	window->draw(toRender);
}