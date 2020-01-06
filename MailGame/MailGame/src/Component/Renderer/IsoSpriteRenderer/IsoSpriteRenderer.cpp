#include "IsoSpriteRenderer.h"
#include "Entity/Entity.h"
#include "Game/Game.h"
#include "Component/Transform/Transform.h"
#include <SFML/Graphics/CircleShape.hpp>
#include "System/Utils/Utils.h"

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
	toRender = Utils::setupBuildingSprite(toRender);
	toRender.setPosition(game->worldToScreenPos(position));
	window->draw(toRender);
}
