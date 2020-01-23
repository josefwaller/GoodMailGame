#include "Construction.h"
#include "Entity/EntityPresets/EntityPresets.h"
#include "Game/Game.h"
#include <math.h>
#include "ResourceLoader/ResourceLoader.h"
#include "ConstructionRecipe//ConstructionRecipe.h"
#include "System/Utils/Utils.h"

std::map<EntityTag, ConstructionRecipe> Construction::recipes = {
	{ EntityTag::PostOffice,
		ConstructionRecipe(
			&EntityPresets::postOffice,
			{
				ResourceLoader::get()->getSprite("buildings/buildings", "postOffice-N"),
				ResourceLoader::get()->getSprite("buildings/buildings", "postOffice-E"),
				ResourceLoader::get()->getSprite("buildings/buildings", "postOffice-S"),
				ResourceLoader::get()->getSprite("buildings/buildings", "postOffice-W")
			},
			ConstructionLayout({
				{ TileType::Empty, TileType::Road }
			})
			)
	}
};
