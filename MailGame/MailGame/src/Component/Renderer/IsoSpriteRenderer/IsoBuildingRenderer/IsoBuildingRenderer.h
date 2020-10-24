#pragma once
#include "Component/Renderer/IsoSpriteRenderer/IsoSpriteRenderer.h"
#include "System/Utils/Utils.h"

// Same as IsoSpriteRenderer, but calls Utils::setupBuildingSprite on every sprite used
class IsoBuildingRenderer : public IsoSpriteRenderer {
public:
	IsoBuildingRenderer(
		IsoSprite sprites,
		sf::Vector2i size = { 1, 1 },
		sf::Vector3f off = { 0.0f, 0.0f, 0.0f });
	virtual void render(sf::RenderWindow* window) override;
private:
	sf::Vector2i size;
};
