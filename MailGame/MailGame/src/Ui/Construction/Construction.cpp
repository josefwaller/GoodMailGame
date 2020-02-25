#include "Construction.h"
#include "Entity/EntityPresets/BuildingPresets/BuildingPresets.h"
#include "Game/Game.h"
#include <math.h>
#include "ResourceLoader/ResourceLoader.h"
#include "ConstructionRecipe//ConstructionRecipe.h"
#include "System/Utils/Utils.h"

std::map<EntityTag, ConstructionRecipe> Construction::recipes = {
	{ EntityTag::PostOffice,
		ConstructionRecipe(
			&BuildingPresets::postOffice,
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
			&BuildingPresets::mailBox,
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
			&BuildingPresets::cargoTruckDepot,
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
	},
	{ EntityTag::TrainStation,
		ConstructionRecipe(
			&BuildingPresets::trainStation,
			{
				ResourceLoader::get()->getSprite("buildings/buildings", "trainstation-N"),
				ResourceLoader::get()->getSprite("buildings/buildings", "trainstation-E"),
				ResourceLoader::get()->getSprite("buildings/buildings", "trainstation-S"),
				ResourceLoader::get()->getSprite("buildings/buildings", "trainstation-W")
			},
			ConstructionLayout({
				{ TileType::Empty }
			})
		)}
	};
