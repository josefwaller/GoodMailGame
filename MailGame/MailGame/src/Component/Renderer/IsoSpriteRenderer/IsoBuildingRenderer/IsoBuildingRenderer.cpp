#include "IsoBuildingRenderer.h"

IsoBuildingRenderer::IsoBuildingRenderer(sf::Sprite n, sf::Sprite e, sf::Sprite s, sf::Sprite w, sf::Vector2f off)
	: IsoSpriteRenderer(n, e, s, w, off) {
	for (auto it = this->sprites.begin(); it != this->sprites.end(); it++) {
		*it = Utils::setupBuildingSprite(*it);
	}
}