#include "EntityPresets.h"

#include "ResourceLoader/ResourceLoader.h"
#include "Game/Game.h"
#include "Entity/Entity.h"
// Include components
#include "Component/Transform/Transform.h"
#include "Component/Renderer/SpriteRenderer/SpriteRenderer.h"
#include "Component/Renderer/IsoSpriteRenderer/IsoSpriteRenderer.h"
#include "Component/Renderer/IsoSpriteRenderer/IsoBuildingRenderer/IsoBuildingRenderer.h"
#include "Component/Controller/PostOfficeController/PostOfficeController.h"
#include "Component/Controller/TruckController/MailTruckController/MailTruckController.h"
#include "Component/Controller/TruckController/CargoTruckController/CargoTruckController.h"
#include "Component/Controller/ResidenceController/ResidenceController.h"
#include "Component/Controller/DepotController/TruckDepotController/TruckDepotController.h"
#include "Component/Controller/DepotController/TrainDepotController/TrainDepotController.h"
#include "Component/ClickBox/RectClickBox/RectClickBox.h"
#include "Component/MailContainer/MailContainer.h"
#include "Component/TransitStop/BasicTransitStop/BasicTransitStop.h"
#include "Component/TransitStop/BuildingTransitStop/BuildingTransitStop.h"

std::shared_ptr<Entity> EntityPresets::residence(Game* g, sf::Vector2f pos, IsoRotation rot) {
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

std::shared_ptr<Entity> EntityPresets::postOffice(Game* g, sf::Vector2f pos, IsoRotation rot) {
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
		new BuildingTransitStop()
	);
}

std::shared_ptr<Entity> EntityPresets::mailBox(Game* g, sf::Vector2f pos, IsoRotation rot) {
	return Entity::createEntity(
		g,
		EntityTag::MailBox,
		new Transform(pos, rot),
		new IsoSpriteRenderer(
			ResourceLoader::get()->getSprite("buildings/buildings", "mailbox", true),
			ResourceLoader::get()->getSprite("buildings/buildings", "mailbox", true),
			ResourceLoader::get()->getSprite("buildings/buildings", "mailbox", true),
			ResourceLoader::get()->getSprite("buildings/buildings", "mailbox", true),
			sf::Vector2f(0.5f, 0.5f)
		),
		nullptr,
		nullptr,
		new MailContainer(),
		new BasicTransitStop()
	);
}

std::shared_ptr<Entity> EntityPresets::mailTruck(
	Game* g,
	sf::Vector2f pos,
	IsoRotation rot,
	MailTruckRoute route,
	std::weak_ptr<Entity> postOffice
) {
	return Entity::createEntity(
		g,
		EntityTag::MailTruck,
		new Transform(pos, rot),
		new IsoSpriteRenderer(
			ResourceLoader::get()->getSprite("vehicles/vehicles", "mailTruck-N"),
			ResourceLoader::get()->getSprite("vehicles/vehicles", "mailTruck-E"),
			ResourceLoader::get()->getSprite("vehicles/vehicles", "mailTruck-S"),
			ResourceLoader::get()->getSprite("vehicles/vehicles", "mailTruck-W")
		),
		new MailTruckController(route, postOffice),
		nullptr,
		new MailContainer()
	);
}

std::shared_ptr<Entity> EntityPresets::cargoTruckDepot(Game* g, sf::Vector2f pos, IsoRotation rot) {
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
		nullptr,
		new MailContainer(),
		new BuildingTransitStop()
	);
}
std::shared_ptr<Entity> EntityPresets::cargoTruck(
	Game* g,
	sf::Vector2f pos,
	IsoRotation rot,
	TransitRoute route,
	std::weak_ptr<Entity> office) {
	return Entity::createEntity(
		g,
		EntityTag::CargoTruck,
		new Transform(pos, rot),
		new IsoSpriteRenderer(
			ResourceLoader::get()->getSprite("vehicles/vehicles", "cargoTruck_NE.png"),
			ResourceLoader::get()->getSprite("vehicles/vehicles", "cargoTruck_SE.png"),
			ResourceLoader::get()->getSprite("vehicles/vehicles", "cargoTruck_SW.png"),
			ResourceLoader::get()->getSprite("vehicles/vehicles", "cargoTruck_NW.png")
		),
		new CargoTruckController(route, office),
		nullptr,
		new MailContainer()
	);
}
std::shared_ptr<Entity> EntityPresets::trainStation(Game* g, sf::Vector2f pos, IsoRotation rot) {
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
		nullptr,
		new MailContainer()
	);
}
std::shared_ptr<Entity> EntityPresets::train(
	Game* g,
	sf::Vector2f pos,
	IsoRotation rot,
	TransitRoute route,
	std::weak_ptr<Entity> depot
) {
	return Entity::createEntity(
		g,
		EntityTag::Train,
		new Transform(pos, rot),
		new IsoSpriteRenderer(
			ResourceLoader::get()->getSprite("vehicles/vehicles", "train-N.png"),
			ResourceLoader::get()->getSprite("vehicles/vehicles", "train-E.png"),
			ResourceLoader::get()->getSprite("vehicles/vehicles", "train-W.png"),
			ResourceLoader::get()->getSprite("vehicles/vehicles", "train-S.png")
		),
		nullptr,
		nullptr,
		new MailContainer()
	);
}
