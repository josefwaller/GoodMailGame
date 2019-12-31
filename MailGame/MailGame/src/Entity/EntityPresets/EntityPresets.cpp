#include "EntityPresets.h"

#include "ResourceLoader/ResourceLoader.h"
#include "Entity/Entity.h"
// Include components
#include "Component/Transform/Transform.h"
#include "Component/Renderer/SpriteRenderer/SpriteRenderer.h"

std::shared_ptr<Entity> EntityPresets::building(Game* g, sf::Vector2f pos, float rotation) {
	return Entity::createEntity(
		g,
		new Transform(pos),
		new SpriteRenderer(ResourceLoader::get()->getSprite("buildings/buildings", "building-SE-3"))
	);
}