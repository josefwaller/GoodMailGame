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
	},
	{ EntityTag::MailBox,
		ConstructionRecipe(
			&EntityPresets::mailBox,
			{
				ResourceLoader::get()->getSprite("buildings/buildings", "mailbox"),
				ResourceLoader::get()->getSprite("buildings/buildings", "mailbox"),
				ResourceLoader::get()->getSprite("buildings/buildings", "mailbox"),
				ResourceLoader::get()->getSprite("buildings/buildings", "mailbox")
			},
			ConstructionLayout({
				{ TileType::Road }
			})
		)
	},
	{ EntityTag::CargoTruckDepot,
		ConstructionRecipe(
			&EntityPresets::cargoTruckDepot,
			{
				ResourceLoader::get()->getSprite("buildings/buildings", "cargoTruckDepot-N"),
				ResourceLoader::get()->getSprite("buildings/buildings", "cargoTruckDepot-E"),
				ResourceLoader::get()->getSprite("buildings/buildings", "cargoTruckDepot-S"),
				ResourceLoader::get()->getSprite("buildings/buildings", "cargoTruckDepot-W")
			},
			ConstructionLayout({
				{TileType::Empty, TileType::Road}
			})
		)
	}
};
