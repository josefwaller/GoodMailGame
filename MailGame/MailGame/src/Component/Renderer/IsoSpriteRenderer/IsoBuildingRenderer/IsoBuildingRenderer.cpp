#include "IsoBuildingRenderer.h"
#include "Game/Game.h"
#include "Entity/Entity.h"
#include "Component/Transform/Transform.h"

IsoBuildingRenderer::IsoBuildingRenderer(
	IsoSprite sprs,
	sf::Vector2i size,
	sf::Vector3f off)
	: IsoSpriteRenderer(sprs, off), size(size) {
}

void IsoBuildingRenderer::render(sf::RenderWindow* window) {
	sf::Sprite toRender = getSpriteToRender();
	toRender = Utils::setupBuildingSprite(toRender);
	sf::Vector3f pos = this->getEntity()->transform->getPosition();
	// Since buildings are origined around where the center of the tile is (~64px from the bottom), we want to draw
	// the sprite at the center of the tile (i.e. pos + (0.5, 0.5)
	Game* g = this->getEntity()->getGame();
	sf::FloatRect bb(toRender.getLocalBounds());
	float originX = 0.0f;
	switch (g->getRotation().getRotation()) {
	case IsoRotation::NORTH:
		originX = bb.width / (size.x + size.y) * size.x;
		pos += sf::Vector3f(size.x, size.y, 0);
		break;
	case IsoRotation::EAST:
		originX = bb.width / (size.x + size.y) * size.y;
		pos += sf::Vector3f(size.x, 0, 0);
		break;
	case IsoRotation::SOUTH:
		originX = bb.width / (size.x + size.y) * size.x;
		break;
	case IsoRotation::WEST:
		originX = bb.width / (size.x + size.y) * size.y;
		pos += sf::Vector3f(0, size.y, 0);
	}
	toRender.setOrigin(originX, bb.height);
	// 32 px is the distance from the bottom of the sprite to the bottom corner
	// Needed because the sprites I'm currently using are rectangular prisms instead of flat squares
	toRender.setPosition(g->worldToScreenPos(
		pos
	) + sf::Vector2f(0, 32));
	window->draw(toRender);
}
