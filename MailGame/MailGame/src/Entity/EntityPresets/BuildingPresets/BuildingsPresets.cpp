#include "BuildingPresets.h"

#include "ResourceLoader/ResourceLoader.h"
#include "Game/Game.h"
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
// Clickboxes
#include "Component/ClickBox/RectClickBox/RectClickBox.h"
// Mail Container
#include "Component/MailContainer/MailContainer.h"
// Transit stops
#include "Component/TransitStop/BasicTransitStop/BasicTransitStop.h"
#include "Component/TransitStop/BuildingTransitStop/BuildingTransitStop.h"

std::shared_ptr<Entity> BuildingPresets::residence(Game* g, sf::Vector3f pos, IsoRotation rot) {
	return Entity::createEntity(
		g,
		EntityTag::Residence,
		new Transform(pos, rot),
		new IsoBuildingRenderer(
			ResourceLoader::get()->getSprite("buildings/buildings", "building-NE-3"),
			ResourceLoader::get()->getSprite("buildings/buildings", "building-SE-3"),
			ResourceLoader::get()->getSprite("buildings/buildings", "building-SW-3"),
			ResourceLoader::get()->getSprite("buildings/buildings", "building-NW-3")
		),
		new ResidenceController(),
		new RectClickBox(sf::FloatRect(
			0,
			0,
			1,
			1)
		),
		nullptr,
		new BuildingTransitStop()
	);
}

std::shared_ptr<Entity> BuildingPresets::postOffice(Game* g, sf::Vector3f pos, IsoRotation rot) {
	addRoadForTransitBuilding(g, sf::Vector3i(pos), rot);
	return Entity::createEntity(
		g,
		EntityTag::PostOffice,
		new Transform(pos, rot),
		new IsoBuildingRenderer(
			ResourceLoader::get()->getSprite("buildings/buildings", "postOffice-N"),
			ResourceLoader::get()->getSprite("buildings/buildings", "postOffice-E"),
			ResourceLoader::get()->getSprite("buildings/buildings", "postOffice-S"),
			ResourceLoader::get()->getSprite("buildings/buildings", "postOffice-W")
		),
		new PostOfficeController(),
		new RectClickBox(sf::FloatRect(0, 0, 1, 1)),
		new MailContainer(),
		new BasicTransitStop()
	);
}

std::shared_ptr<Entity> BuildingPresets::mailBox(Game* g, sf::Vector3f pos, IsoRotation rot) {
	return Entity::createEntity(
		g,
		EntityTag::MailBox,
		new Transform(pos, rot),
		new IsoSpriteRenderer(
			ResourceLoader::get()->getSprite("buildings/buildings", "mailbox", true),
			ResourceLoader::get()->getSprite("buildings/buildings", "mailbox", true),
			ResourceLoader::get()->getSprite("buildings/buildings", "mailbox", true),
			ResourceLoader::get()->getSprite("buildings/buildings", "mailbox", true),
			sf::Vector3f(0.5f, 0.5f, 0)
		),
		nullptr,
		nullptr,
		new MailContainer(),
		new BasicTransitStop()
	);
}



std::shared_ptr<Entity> BuildingPresets::cargoTruckDepot(Game* g, sf::Vector3f pos, IsoRotation rot) {
	addRoadForTransitBuilding(g, sf::Vector3i(pos), rot);
	return Entity::createEntity(
		g,
		EntityTag::CargoTruckDepot,
		new Transform(pos, rot),
		new IsoBuildingRenderer(
			ResourceLoader::get()->getSprite("buildings/buildings", "cargoTruckDepot-N"),
			ResourceLoader::get()->getSprite("buildings/buildings", "cargoTruckDepot-E"),
			ResourceLoader::get()->getSprite("buildings/buildings", "cargoTruckDepot-S"),
			ResourceLoader::get()->getSprite("buildings/buildings", "cargoTruckDepot-W")
		),
		new TruckDepotController(),
		new RectClickBox(sf::FloatRect(0, 0, 1, 1)),
		new MailContainer(),
		new BasicTransitStop()
	);
}

std::shared_ptr<Entity> BuildingPresets::trainStation(Game* g, sf::Vector3f pos, IsoRotation rot) {
	// Add road into train station
	addRoadForTransitBuilding(g, sf::Vector3i(pos), rot);
	return Entity::createEntity(
		g,
		EntityTag::TrainStation,
		new Transform(pos, rot),
		new IsoBuildingRenderer(
			ResourceLoader::get()->getSprite("buildings/buildings", "trainstation-N"),
			ResourceLoader::get()->getSprite("buildings/buildings", "trainstation-E"),
			ResourceLoader::get()->getSprite("buildings/buildings", "trainstation-S"),
			ResourceLoader::get()->getSprite("buildings/buildings", "trainstation-W")
		),
		new TrainDepotController(),
		new RectClickBox(sf::FloatRect(0, 0, 1, 1)),
		new MailContainer(),
		new BasicTransitStop()
	);
}
std::shared_ptr<Entity> BuildingPresets::airport(Game* g, sf::Vector3f pos, IsoRotation rot) {
	addRoadForTransitBuilding(g, sf::Vector3i(pos), rot);
	return Entity::createEntity(
		g,
		EntityTag::Airport,
		new Transform(pos, rot),
		new IsoBuildingRenderer(
			ResourceLoader::get()->getSprite("buildings/buildings", "airport-N"),
			ResourceLoader::get()->getSprite("buildings/buildings", "airport-E"),
			ResourceLoader::get()->getSprite("buildings/buildings", "airport-S"),
			ResourceLoader::get()->getSprite("buildings/buildings", "airport-W")
		),
		new PlaneDepotController(),
		new RectClickBox(sf::FloatRect(0, 0, 1, 1)),
		new MailContainer(),
		new BasicTransitStop()
	);
}

void BuildingPresets::addRoadForTransitBuilding(Game* g, sf::Vector3i pos, IsoRotation rot) {
	sf::Vector3i inFront = pos + sf::Vector3i(rot.getUnitVector3D());
	g->getGameMap()->addRoadInDirection(inFront.x, inFront.y, rot + 2);
	g->getGameMap()->addRoadInDirection((size_t)pos.x, (size_t)pos.y, rot);
}
