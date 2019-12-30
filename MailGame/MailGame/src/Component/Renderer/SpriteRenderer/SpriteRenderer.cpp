#include "SpriteRenderer.h"
#include "Entity\Entity.h"
#include "Component\Transform\Transform.h"

SpriteRenderer::SpriteRenderer(std::weak_ptr<Entity> e, sf::Sprite spr) : Component(e), sprite(spr) {

}

void SpriteRenderer::render(sf::RenderWindow* window) {
	if (std::shared_ptr<Transform> trans = this->getEntity()->transform.lock()) {
		this->sprite.setPosition(trans->getPosition());
	}
	window->draw(this->sprite);
}