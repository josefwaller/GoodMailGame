#include "SpriteRenderer.h"
#include "Entity\Entity.h"
#include "Game/Game.h"
#include "Component\Transform\Transform.h"

SpriteRenderer::SpriteRenderer(sf::Sprite spr) : sprite(spr) {

}

void SpriteRenderer::render(sf::RenderWindow* window) {
	std::shared_ptr<Transform> trans = this->getEntity()->transform;
	if (trans) {
		this->sprite.setPosition(this->getEntity()->getGame()->worldToScreenPos(trans->getPosition()));
	}
	// Center the sprite on the bottom center
	this->sprite.setOrigin(this->sprite.getLocalBounds().width / 2, this->sprite.getLocalBounds().height);
	window->draw(this->sprite);
}
