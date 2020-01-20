#include "EntityPresets.h"

#include "ResourceLoader/ResourceLoader.h"
#include "Game/Game.h"
#include "Entity/Entity.h"
// Include components
#include "Component/Transform/Transform.h"
#include "Component/Renderer/SpriteRenderer/SpriteRenderer.h"
#include "Component/Renderer/IsoSpriteRenderer/IsoSpriteRenderer.h"
#include "Component/Controller/PostOfficeController/PostOfficeController.h"
#include "Component/ClickBox/RectClickBox/RectClickBox.h"

std::shared_ptr<Entity> EntityPresets::building(Game* g, sf::Vector2f pos, IsoRotation rot) {
	return Entity::createEntity(
		g,
		EntityTag::Building,
		new Transform(pos, rot),
		new IsoSpriteRenderer(
			ResourceLoader::get()->getSprite("buildings/buildings", "building-NE-3"),
			ResourceLoader::get()->getSprite("buildings/buildings", "building-SE-3"),
			ResourceLoader::get()->getSprite("buildings/buildings", "building-SW-3"),
			ResourceLoader::get()->getSprite("buildings/buildings", "building-NW-3")
		),
		nullptr,
		new RectClickBox(sf::FloatRect(
			-1,
			-1,
			1,
			1)
		)
	);
}

std::shared_ptr<Entity> EntityPresets::postOffice(Game* g, sf::Vector2f pos, IsoRotation rot) {
	return Entity::createEntity(
		g,
		EntityTag::PostOffice,
		new Transform(pos, rot),
		new IsoSpriteRenderer(
			ResourceLoader::get()->getSprite("buildings/buildings", "postOffice-N"),
			ResourceLoader::get()->getSprite("buildings/buildings", "postOffice-E"),
			ResourceLoader::get()->getSprite("buildings/buildings", "postOffice-S"),
			ResourceLoader::get()->getSprite("buildings/buildings", "postOffice-W")
		),
		new PostOfficeController()
	);
}
