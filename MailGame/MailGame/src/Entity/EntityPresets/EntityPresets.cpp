#include "EntityPresets.h"

#include "ResourceLoader/ResourceLoader.h"
#include "Entity/Entity.h"
// Include components
#include "Component/Transform/Transform.h"
#include "Component/Renderer/SpriteRenderer/SpriteRenderer.h"
#include "Component/Renderer/IsoSpriteRenderer/IsoSpriteRenderer.h"

std::shared_ptr<Entity> EntityPresets::building(Game* g, sf::Vector2f pos, IsoRotation rot) {
	return Entity::createEntity(
		g,
		new Transform(pos, rot),
		new IsoSpriteRenderer(
			ResourceLoader::get()->getSprite("buildings/buildings", "building-NE-3"),
			ResourceLoader::get()->getSprite("buildings/buildings", "building-SE-3"),
			ResourceLoader::get()->getSprite("buildings/buildings", "building-SW-3"),
			ResourceLoader::get()->getSprite("buildings/buildings", "building-NW-3"))
	);
	auto x = 0;
}
