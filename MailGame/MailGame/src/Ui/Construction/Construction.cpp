#include "Construction.h"
#include "Entity/EntityPresets/EntityPresets.h"
#include "Game/Game.h"
#include <math.h>

std::map<EntityTag, Construction::Recipe> Construction::recipes = {
	{ EntityTag::PostOffice, {
		[](Game* g, sf::Vector2f pos, IsoRotation rot) {
			sf::Vector2i tilesPos(round(pos.x), round(pos.y));
			std::shared_ptr<Entity> e = EntityPresets::postOffice(g, sf::Vector2f(tilesPos), rot);
			g->getGameMap()->setBuildingForTile(tilesPos.x, tilesPos.y, e);
			return e;
		},
		[](Game* g, sf::Vector2f pos, IsoRotation rot) {
			return true;
		}
	}}
};

