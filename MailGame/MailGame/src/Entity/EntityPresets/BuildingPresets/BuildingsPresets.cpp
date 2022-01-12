#include "BuildingPresets.h"

#include "ResourceLoader/ResourceLoader.h"
#include "Game/Game.h"
#include "GameMap/GameMap.h"
#include "Entity/Entity.h"

// Compoennts
// Transform
#include "Component/Transform/Transform.h"
// Renderers
#include "Component/Renderer/IsoSpriteRenderer/IsoBuildingRenderer/IsoBuildingRenderer.h"
// Controllers
#include "Component/Controller/PostOfficeController/PostOfficeController.h"
#include "Component/Controller/ResidenceController/ResidenceController.h"
#include "Component/Controller/DepotController/TruckDepotController/TruckDepotController.h"
#include "Component/Controller/DepotController/TrainDepotController/TrainDepotController.h"
#include "Component/Controller/DepotController/PlaneDepotController/PlaneDepotController.h"
#include "Component/Controller/DepotController/BoatDepotController/BoatDepotController.h"
#include "Component/Controller/RailwaySwitchController/RailwaySwitchController.h"
// Pathfinders
#include "Component/Pathfinder/RoadPathfinder/RoadPathfinder.h"
#include "Component/Pathfinder/RailsPathfinder/RailsPathfinder.h"
#include "Component/Pathfinder/AirPathfinder/AirPathfinder.h"
// Clickboxes
#include "Component/ClickBox/RectClickBox/RectClickBox.h"
#include "Component/ClickBox/PolygonClickBox/PolygonClickBox.h"
// Mail Container
#include "Component/MailContainer/MailContainer.h"
// Pathfinders
#include "Component/Pathfinder/AirPathfinder/AirPathfinder.h"
#include "Component/Pathfinder/RailsPathfinder/RailsPathfinder.h"
#include "Component/Pathfinder/RoadPathfinder/RoadPathfinder.h"

PolygonClickBox getOneTileBuildingHitbox(sf::Vector3f pos) {
	const float HEIGHT = 40;
	return PolygonClickBox(Polygon({
		sf::Vector2f(-Game::TILE_WIDTH, -HEIGHT),
		sf::Vector2f(-Game::TILE_WIDTH, 0),
		sf::Vector2f(0, Game::TILE_HEIGHT),
		sf::Vector2f(Game::TILE_WIDTH, 0),
		sf::Vector2f(Game::TILE_WIDTH, -HEIGHT),
		sf::Vector2f(0, -Game::TILE_HEIGHT - HEIGHT)
	}), sf::Vector3f(0.5, 0.5, 0));
}

std::shared_ptr<Entity> BuildingPresets::residence(Game* g, sf::Vector3f pos, IsoRotation rot) {
	return Entity::createEntity(
		g,
		EntityTag::Residence,
		new Transform(pos, rot),
		new IsoBuildingRenderer(IsoSprite(
			ResourceLoader::get()->getSprite("buildings/buildings", "building-NE-3"),
			ResourceLoader::get()->getSprite("buildings/buildings", "building-SE-3"),
			ResourceLoader::get()->getSprite("buildings/buildings", "building-SW-3"),
			ResourceLoader::get()->getSprite("buildings/buildings", "building-NW-3")
		)),
		new ResidenceController(g->getTime()),
		nullptr,
		nullptr,
		new TransitStop(getDefaultBuildingTransitStop(pos, rot))
	);
}

std::shared_ptr<Entity> BuildingPresets::postOffice(Game* g, sf::Vector3f pos, IsoRotation rot) {
	addRoadForTransitBuilding(g, sf::Vector3i(pos), rot);
	return Entity::createEntity(
		g,
		EntityTag::PostOffice,
		new Transform(pos, rot),
		new IsoBuildingRenderer(IsoSprite(
			ResourceLoader::get()->getSprite("buildings/buildings", "postOffice-N"),
			ResourceLoader::get()->getSprite("buildings/buildings", "postOffice-E"),
			ResourceLoader::get()->getSprite("buildings/buildings", "postOffice-S"),
			ResourceLoader::get()->getSprite("buildings/buildings", "postOffice-W")
		)),
		new PostOfficeController(),
		new PolygonClickBox(getOneTileBuildingHitbox(pos)),
		new MailContainer(),
		new TransitStop(getDefaultBuildingTransitStop(pos, rot)),
		new RoadPathfinder()
	);
}

std::shared_ptr<Entity> BuildingPresets::mailBox(Game* g, sf::Vector3f pos, IsoRotation rot) {
	TransitStop::CarStop stop = {
		pos,
		{}
	};
	return Entity::createEntity(
		g,
		EntityTag::MailBox,
		new Transform(pos, rot),
		new IsoSpriteRenderer(
			IsoSprite(
				ResourceLoader::get()->getSprite("buildings/buildings", "mailbox", true),
				ResourceLoader::get()->getSprite("buildings/buildings", "mailbox", true),
				ResourceLoader::get()->getSprite("buildings/buildings", "mailbox", true),
				ResourceLoader::get()->getSprite("buildings/buildings", "mailbox", true)
			),
			sf::Vector3f(0.5f, 0.5f, 0)
		),
		nullptr,
		nullptr,
		new MailContainer(),
		new TransitStop(stop)
	);
}

std::shared_ptr<Entity> BuildingPresets::cargoTruckDepot(Game* g, sf::Vector3f pos, IsoRotation rot) {
	addRoadForTransitBuilding(g, sf::Vector3i(pos), rot);
	return Entity::createEntity(
		g,
		EntityTag::CargoTruckDepot,
		new Transform(pos, rot),
		new IsoBuildingRenderer(IsoSprite(
			ResourceLoader::get()->getSprite("buildings/buildings", "cargoTruckDepot-N"),
			ResourceLoader::get()->getSprite("buildings/buildings", "cargoTruckDepot-E"),
			ResourceLoader::get()->getSprite("buildings/buildings", "cargoTruckDepot-S"),
			ResourceLoader::get()->getSprite("buildings/buildings", "cargoTruckDepot-W")
		)),
		new TruckDepotController(),
		new PolygonClickBox(getOneTileBuildingHitbox(pos)),
		new MailContainer(),
		new TransitStop(getDefaultBuildingTransitStop(pos, rot)),
		new RoadPathfinder()
	);
}

std::shared_ptr<Entity> BuildingPresets::trainStation(Game* g, sf::Vector3f pos, IsoRotation rot) {
	TransitStop::CarStop carStop = getDefaultBuildingTransitStop(pos, rot);
	TransitStop::TrainStop trainStop = {
		pos,
		rot - 1
	};

	return Entity::createEntity(
		g,
		EntityTag::TrainStation,
		new Transform(pos, rot),
		new IsoBuildingRenderer(IsoSprite(
			ResourceLoader::get()->getSprite("buildings/buildings", "trainstation-N"),
			ResourceLoader::get()->getSprite("buildings/buildings", "trainstation-E"),
			ResourceLoader::get()->getSprite("buildings/buildings", "trainstation-S"),
			ResourceLoader::get()->getSprite("buildings/buildings", "trainstation-W")
		)),
		new TrainDepotController(),
		new PolygonClickBox(getOneTileBuildingHitbox(pos)),
		new MailContainer(),
		new TransitStop(carStop, trainStop),
		new RailsPathfinder()
	);
}
std::shared_ptr<Entity> BuildingPresets::airport(Game* g, sf::Vector3f pos, IsoRotation rot) {
	auto e = Entity::createEntity(
		g,
		EntityTag::Airport,
		new Transform(pos, rot),
		new IsoBuildingRenderer(
			IsoSprite(
				ResourceLoader::get()->getSprite("buildings/buildings", "cargoTruckDepot-N"),
				ResourceLoader::get()->getSprite("buildings/buildings", "cargoTruckDepot-E"),
				ResourceLoader::get()->getSprite("buildings/buildings", "cargoTruckDepot-S"),
				ResourceLoader::get()->getSprite("buildings/buildings", "cargoTruckDepot-W")
			)
		),
		new PlaneDepotController(),
		new PolygonClickBox(getOneTileBuildingHitbox(pos)),
		new MailContainer(),
		nullptr,
		new AirPathfinder()
	);
	setTilesToBuilding(g, e, sf::IntRect((int)pos.x, (int)pos.y, 1, 1));
	return e;
}
std::shared_ptr<Entity> BuildingPresets::warehouse(Game* g, sf::Vector3f pos, IsoRotation rot) {
	auto e = Entity::createEntity(
		g,
		EntityTag::Warehouse,
		new Transform(pos, rot),
		new IsoBuildingRenderer(
			IsoSprite(
				ResourceLoader::get()->getSprite("buildings/warehouse", "warehouse"),
				ResourceLoader::get()->getSprite("buildings/warehouse", "warehouse"),
				ResourceLoader::get()->getSprite("buildings/warehouse", "warehouse"),
				ResourceLoader::get()->getSprite("buildings/warehouse", "warehouse")
			),
			sf::Vector2i(2, 2),
			sf::Vector3f(-1, -0.5, 0)
		),
		nullptr,
		new PolygonClickBox(
			Polygon({
				sf::Vector2f(-2 * Game::TILE_WIDTH, 0),
				sf::Vector2f(0, -2 * Game::TILE_HEIGHT),
				sf::Vector2f(2 * Game::TILE_WIDTH, 0),
				sf::Vector2f(0, 2 * Game::TILE_HEIGHT)
			}),
			sf::Vector3f(1, 1, 0)
		),
		new MailContainer()
	);
	setTilesToBuilding(g, e, sf::IntRect(floor(pos.x), floor(pos.y), 2, 2));
	return e;
}

std::shared_ptr<Entity> BuildingPresets::boatDepot(Game* g, sf::Vector3f pos, IsoRotation rot) {
	return Entity::createEntity(
		g,
		EntityTag::BoatDepot,
		new Transform(pos, rot),
		new IsoBuildingRenderer(
			IsoSprite(
				ResourceLoader::get()->getSprite("buildings/buildings", "cargoTruckDepot-N"),
				ResourceLoader::get()->getSprite("buildings/buildings", "cargoTruckDepot-E"),
				ResourceLoader::get()->getSprite("buildings/buildings", "cargoTruckDepot-S"),
				ResourceLoader::get()->getSprite("buildings/buildings", "cargoTruckDepot-W")
			)
		),
		new BoatDepotController(),
		new PolygonClickBox(getOneTileBuildingHitbox(pos)),
		new MailContainer()
	);
}

std::shared_ptr<Entity> BuildingPresets::carDock(Game* g, sf::Vector3f pos, IsoRotation rot) {
	addRoadForTransitBuilding(g, sf::Vector3i(pos), rot);
	return dock(g, pos, rot, EntityTag::CarDock, sf::Color(255, 0, 238));
}
std::shared_ptr<Entity> BuildingPresets::trainDock(Game* g, sf::Vector3f pos, IsoRotation rot) {
	return dock(g, pos, rot, EntityTag::TrainDock, sf::Color(255, 174, 0));
}
std::shared_ptr<Entity> BuildingPresets::airplaneDock(Game* g, sf::Vector3f pos, IsoRotation rot) {
	g->getGameMap()->addAirplaneRoad(floor(pos.x), floor(pos.y), AirplaneRoad(rot));
	return dock(g, pos, rot, EntityTag::AirplaneDock, sf::Color(52, 235, 229));
}
std::shared_ptr<Entity> BuildingPresets::boatDock(Game* g, sf::Vector3f pos, IsoRotation rot) {
	return dock(g, pos, rot, EntityTag::BoatDock, sf::Color(252, 248, 3));
}

std::shared_ptr<Entity> BuildingPresets::dock(Game* g, sf::Vector3f pos, IsoRotation rot, EntityTag tag, sf::Color color) {
	auto e = Entity::createEntity(
		g,
		tag,
		new Transform(pos, rot),
		new IsoBuildingRenderer(IsoSprite(
			ResourceLoader::get()->getSprite("buildings/buildings", "trainstation-N"),
			ResourceLoader::get()->getSprite("buildings/buildings", "trainstation-E"),
			ResourceLoader::get()->getSprite("buildings/buildings", "trainstation-S"),
			ResourceLoader::get()->getSprite("buildings/buildings", "trainstation-W"),
			color
		))
	);
	setTilesToBuilding(g, e, sf::IntRect(floor(pos.x), floor(pos.y), 1, 1));
	return e;
}

void BuildingPresets::addRoadForTransitBuilding(Game* g, sf::Vector3i pos, IsoRotation rot) {
	sf::Vector3i inFront = pos + sf::Vector3i(rot.getUnitVector3D());
	g->getGameMap()->addRoadInDirection(inFront.x, inFront.y, rot.getReverse());
	g->getGameMap()->addRoadInDirection((size_t)pos.x, (size_t)pos.y, rot);
}

void BuildingPresets::setTilesToBuilding(Game* g, std::weak_ptr<Entity> e, sf::IntRect pos) {
	for (size_t x = pos.left; x < pos.left + pos.width; x++) {
		for (size_t y = pos.top; y < pos.top + pos.height; y++) {
			g->getGameMap()->setBuildingForTile(x, y, e);
		}
	}
}

TransitStop::CarStop BuildingPresets::getDefaultBuildingTransitStop(sf::Vector3f pos, IsoRotation rot) {
	return {
		pos,
		{ rot}
	};
}