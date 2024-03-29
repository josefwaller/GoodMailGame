#include "Construction.h"
#include "Entity/EntityPresets/BuildingPresets/BuildingPresets.h"
#include "Game/Game.h"
#include <math.h>
#include "ResourceLoader/ResourceLoader.h"
#include "ConstructionRecipe//ConstructionRecipe.h"
#include "System/Utils/Utils.h"

// Add a road at the position given in the direction of the rotation given
void addRoad(Game* g, sf::Vector3f pos, IsoRotation rot) {
	// rot.getReverse since rot is the outgoing direction, but we set roads using the ingoing directions
	g->getGameMap()->addRoad((size_t)floorf(pos.x), (size_t)floorf(pos.y), Road(rot.getReverse()));
}

std::map<EntityTag, ConstructionRecipe> Construction::recipes = {
	{ EntityTag::PostOffice,
		ConstructionRecipe(
			"Post Office",
			[&](Game * g, sf::Vector3f pos, IsoRotation rot) {
				addRoad(g, pos, rot);
				return BuildingPresets::postOffice(g, pos, rot);
			},
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
			"Train Depot",
			[&](Game* g, sf::Vector3f pos, IsoRotation rot) {
				g->getGameMap()->addRailTrack((size_t)pos.x, (size_t)pos.y, rot, rot, true);
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
			[&](Game* g, sf::Vector3f pos, IsoRotation rot) {
				sf::Vector2i p = Utils::toVector2i(pos);
				g->getGameMap()->addAirplaneRoad(p.x, p.y, AirplaneRoad(rot));
				return BuildingPresets::airport(g, pos, rot);
			},
			IsoSprite(
				ResourceLoader::get()->getSprite("buildings/buildings", "cargoTruckDepot-N"),
				ResourceLoader::get()->getSprite("buildings/buildings", "cargoTruckDepot-E"),
				ResourceLoader::get()->getSprite("buildings/buildings", "cargoTruckDepot-S"),
				ResourceLoader::get()->getSprite("buildings/buildings", "cargoTruckDepot-W")
			),
			ConstructionLayout({
				{ TileType::Land }
			}),
			Technology::Airplanes
		)
	},
	{
		EntityTag::Warehouse,
		ConstructionRecipe(
			"Warehouse",
			&BuildingPresets::warehouse,
			IsoSprite(
				ResourceLoader::get()->getSprite("buildings/warehouse", "warehouse"),
				ResourceLoader::get()->getSprite("buildings/warehouse", "warehouse"),
				ResourceLoader::get()->getSprite("buildings/warehouse", "warehouse"),
				ResourceLoader::get()->getSprite("buildings/warehouse", "warehouse")
			),
			ConstructionLayout({
				{ TileType::Land, TileType::Land },
				{ TileType::Land, TileType::Land }
			}),
			Technology::Default
		)
	},
	{ EntityTag::CarDock,
		ConstructionRecipe(
			"CarDock",
			&BuildingPresets::carDock,
			IsoSprite(
				ResourceLoader::get()->getSprite("buildings/buildings", "trainstation-N"),
				ResourceLoader::get()->getSprite("buildings/buildings", "trainstation-E"),
				ResourceLoader::get()->getSprite("buildings/buildings", "trainstation-S"),
				ResourceLoader::get()->getSprite("buildings/buildings", "trainstation-W")
			),
			ConstructionLayout({{ TileType::Land }}),
			Technology::Default
		)
	},
	{ EntityTag::TrainDock,
		ConstructionRecipe(
			"TrainDock",
			&BuildingPresets::trainDock,
			IsoSprite(
				ResourceLoader::get()->getSprite("buildings/buildings", "trainstation-N"),
				ResourceLoader::get()->getSprite("buildings/buildings", "trainstation-E"),
				ResourceLoader::get()->getSprite("buildings/buildings", "trainstation-S"),
				ResourceLoader::get()->getSprite("buildings/buildings", "trainstation-W")
			),
			ConstructionLayout({{ TileType::Land }}),
			Technology::Trains
		)
	},
	{ EntityTag::AirplaneDock,
		ConstructionRecipe(
			"AirplaneDock",
			[&](Game* g, sf::Vector3f pos, IsoRotation rot) {
				g->getGameMap()->addAirplaneRoad((size_t)pos.x, (size_t)pos.y, AirplaneRoad(rot));
				return BuildingPresets::airplaneDock(g, pos, rot);
			},
			IsoSprite(
				ResourceLoader::get()->getSprite("buildings/buildings", "trainstation-N"),
				ResourceLoader::get()->getSprite("buildings/buildings", "trainstation-E"),
				ResourceLoader::get()->getSprite("buildings/buildings", "trainstation-S"),
				ResourceLoader::get()->getSprite("buildings/buildings", "trainstation-W")
			),
			ConstructionLayout({{ TileType::Land }}),
			Technology::Airplanes
		)
	},
	{
		EntityTag::BoatDock,
		ConstructionRecipe(
			"Boat Dock",
			&BuildingPresets::boatDock,
			IsoSprite(
				ResourceLoader::get()->getSprite("buildings/buildings", "trainstation-N"),
				ResourceLoader::get()->getSprite("buildings/buildings", "trainstation-E"),
				ResourceLoader::get()->getSprite("buildings/buildings", "trainstation-S"),
				ResourceLoader::get()->getSprite("buildings/buildings", "trainstation-W")
			),
			ConstructionLayout({{ TileType::Land}}),
			Technology::Default
		)
	},
		{
			EntityTag::BoatDepot,
			ConstructionRecipe(
				"Boat Depot",
				[&](Game* g, sf::Vector3f pos, IsoRotation rot) {
					pos.z = GameMap::SEA_LEVEL;
					return BuildingPresets::boatDepot(g, pos, rot);
				},
				IsoSprite(
					ResourceLoader::get()->getSprite("buildings/buildings", "trainstation-N"),
					ResourceLoader::get()->getSprite("buildings/buildings", "trainstation-E"),
					ResourceLoader::get()->getSprite("buildings/buildings", "trainstation-S"),
					ResourceLoader::get()->getSprite("buildings/buildings", "trainstation-W")
				),
				ConstructionLayout({{ TileType::Water }}),
				Technology::Default
			)
		}
};