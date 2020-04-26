#include "IsoBuildingRenderer.h"
#include "Game/Game.h"
#include "Entity/Entity.h"
#include "Component/Transform/Transform.h"

IsoBuildingRenderer::IsoBuildingRenderer(sf::Sprite n, sf::Sprite e, sf::Sprite s, sf::Sprite w, sf::Vector3f off)
	: IsoSpriteRenderer(n, e, s, w, off) {
	for (auto it = this->sprites.begin(); it != this->sprites.end(); it++) {
		*it = Utils::setupBuildingSprite(*it);
	}
}

void IsoBuildingRenderer::render(sf::RenderWindow* window) {
	sf::Sprite toRender = getSpriteToRender();
	// Since buildings are origined around where the center of the tile is (~64px from the bottom), we want to draw
	// the sprite at the center of the tile (i.e. pos + (0.5, 0.5)
	Game* g = this->getEntity()->getGame();
	toRender.setPosition(g->worldToScreenPos(
		this->getEntity()->transform->getPosition() + sf::Vector3f(0.5f, 0.5f, 0) + this->offset)
	);
	window->draw(toRender);
}
