#pragma once
#include "Component/Renderer/IsoSpriteRenderer/IsoSpriteRenderer.h"
#include "System/Utils/Utils.h"

// Same as IsoSpriteRenderer, but calls Utils::setupBuildingSprite on every sprite used
class IsoBuildingRenderer : public IsoSpriteRenderer {
public:
	IsoBuildingRenderer(sf::Sprite n, sf::Sprite e, sf::Sprite s, sf::Sprite w, sf::Vector2f off = { 0.0f, 0.0f });
};
