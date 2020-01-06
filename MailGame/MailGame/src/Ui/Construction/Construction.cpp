#include "Construction.h"
#include "Entity/EntityPresets/EntityPresets.h"
#include "Game/Game.h"
#include <math.h>
#include "ResourceLoader/ResourceLoader.h"
#include "System/Utils/Utils.h"

std::map<EntityTag, Construction::Recipe> Construction::recipes = {
	{ EntityTag::PostOffice, {
		[](Game* g, sf::Vector2f pos, IsoRotation rot) {
			sf::Vector2i tilesPos((int)round(pos.x), (int)round(pos.y));
			std::shared_ptr<Entity> e = EntityPresets::postOffice(g, sf::Vector2f(tilesPos), rot);
			g->getGameMap()->setBuildingForTile(tilesPos.x, tilesPos.y, e);
			return e;
		},
		[](Game* g, sf::Vector2f pos, IsoRotation rot) {
			return true;
		},
		[](Game* g, sf::Vector2f pos, IsoRotation rot) {
			pos = sf::Vector2f(round(pos.x), round(pos.y));
			sf::Sprite toReturn = ResourceLoader::get()->getSprite("buildings/buildings", "postOffice-N");
			toReturn = Utils::setupBuildingSprite(toReturn);
			pos = g->worldToScreenPos(pos);
			toReturn.setPosition(pos);
			return toReturn;
		}
	}}
};

