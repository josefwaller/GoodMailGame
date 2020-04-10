#include "VehiclePresets.h"
#include "Entity/Entity.h"
#include "ResourceLoader/ResourceLoader.h"
// Components
// Transform
#include "Component/Transform/Transform.h"
// Renderers
#include "Component/Renderer/IsoSpriteRenderer/IsoSpriteRenderer.h"
// Controllers
#include "Component/Controller/VehicleController/MailTruckController/MailTruckController.h"
#include "Component/Controller/VehicleController/CargoTruckController/CargoTruckController.h"
#include "Component/Controller/TrainController/TrainController.h"
// Mail Container
#include "Component/MailContainer/MailContainer.h"
// Pathfinders
#include "Component/Pathfinder/RoadPathfinder/RoadPathfinder.h"

std::shared_ptr<Entity> VehiclePresets::mailTruck(
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
		new MailContainer(),
		nullptr,
		new RoadPathfinder()
	);
}

std::shared_ptr<Entity> VehiclePresets::cargoTruck(
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
		new MailContainer(),
		nullptr,
		new RoadPathfinder()
	);
}

std::shared_ptr<Entity> VehiclePresets::train(
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
		new TrainController(route, depot),
		nullptr,
		new MailContainer()
	);
}
