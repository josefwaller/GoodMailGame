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
			"Post Office",
			&BuildingPresets::postOffice,
			IsoSprite(
				ResourceLoader::get()->getSprite("buildings/buildings", "postOffice-N"),
				ResourceLoader::get()->getSprite("buildings/buildings", "postOffice-E"),
				ResourceLoader::get()->getSprite("buildings/buildings", "postOffice-S"),
				ResourceLoader::get()->getSprite("buildings/buildings", "postOffice-W")
			),
			ConstructionLayout({
				{ TileType::Land }
			})
			)
	},
	{ EntityTag::MailBox,
		ConstructionRecipe(
			"Mail Box",
			&BuildingPresets::mailBox,
			IsoSprite(
				ResourceLoader::get()->getSprite("buildings/buildings", "mailbox"),
				ResourceLoader::get()->getSprite("buildings/buildings", "mailbox"),
				ResourceLoader::get()->getSprite("buildings/buildings", "mailbox"),
				ResourceLoader::get()->getSprite("buildings/buildings", "mailbox")
			),
			ConstructionLayout({
				{ TileType::Land }
			})
		)
	},
	{ EntityTag::CargoTruckDepot,
		ConstructionRecipe(
			"Cargo Truck Depot",
			&BuildingPresets::cargoTruckDepot,
			IsoSprite(
				ResourceLoader::get()->getSprite("buildings/buildings", "cargoTruckDepot-N"),
				ResourceLoader::get()->getSprite("buildings/buildings", "cargoTruckDepot-E"),
				ResourceLoader::get()->getSprite("buildings/buildings", "cargoTruckDepot-S"),
				ResourceLoader::get()->getSprite("buildings/buildings", "cargoTruckDepot-W")
			),
			ConstructionLayout({
				{TileType::Land }
			}),
			Technology::CargoTrucks
		)
	},
	{ EntityTag::TrainStation,
		ConstructionRecipe(
			"Train Station",
			[&](Game* g, sf::Vector3f pos, IsoRotation rot) {
				g->getGameMap()->addRailTrack((size_t)pos.x, (size_t)pos.y, rot + 1, rot + 3);
				return BuildingPresets::trainStation(g, pos, rot);
			},
			IsoSprite(
				ResourceLoader::get()->getSprite("buildings/buildings", "trainstation-N"),
				ResourceLoader::get()->getSprite("buildings/buildings", "trainstation-E"),
				ResourceLoader::get()->getSprite("buildings/buildings", "trainstation-S"),
				ResourceLoader::get()->getSprite("buildings/buildings", "trainstation-W")
			),
			ConstructionLayout({
				{ TileType::Land }
			}),
			Technology::Trains
		)
	},
	{ EntityTag::Airport,
		ConstructionRecipe(
			"Airport",
			&BuildingPresets::airport,
			IsoSprite(
				ResourceLoader::get()->getSprite("buildings/airport", "airport-N"),
				ResourceLoader::get()->getSprite("buildings/airport", "airport-E"),
				ResourceLoader::get()->getSprite("buildings/airport", "airport-S"),
				ResourceLoader::get()->getSprite("buildings/airport", "airport-W")
			),
			ConstructionLayout({
				{ TileType::Land }
			}),
			Technology::Airplanes
		)
	}
};
