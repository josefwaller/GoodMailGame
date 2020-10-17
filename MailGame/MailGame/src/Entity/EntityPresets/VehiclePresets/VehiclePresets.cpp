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
#include "Component/Controller/VehicleController/CargoVehicleController/CargoVehicleController.h"
// Mail Container
#include "Component/MailContainer/MailContainer.h"
// Pathfinders
#include "Component/Pathfinder/RoadPathfinder/RoadPathfinder.h"
#include "Component/Pathfinder/RailsPathfinder/RailsPathfinder.h"
#include "Component/Pathfinder/AirPathfinder/AirPathfinder.h"
#include "Game/Game.h"

std::shared_ptr<Entity> VehiclePresets::mailTruck(
	Game* g,
	sf::Vector3f pos,
	IsoRotation rot,
	MailTruckRoute route,
	std::weak_ptr<Entity> postOffice
) {
	return Entity::createEntity(
		g,
		EntityTag::MailTruck,
		new Transform(pos, rot),
		new IsoSpriteRenderer(
			ResourceLoader::get()->getSprite("vehicles/vehicles", "mailTruck-N", true),
			ResourceLoader::get()->getSprite("vehicles/vehicles", "mailTruck-E", true),
			ResourceLoader::get()->getSprite("vehicles/vehicles", "mailTruck-S", true),
			ResourceLoader::get()->getSprite("vehicles/vehicles", "mailTruck-W", true),
			sf::Vector3f(0.5f, 0.5f, 0)
		),
		new MailTruckController(route, postOffice, g->getTime()),
		nullptr,
		new MailContainer(),
		nullptr,
		new RoadPathfinder()
	);
}

std::shared_ptr<Entity> VehiclePresets::cargoTruck(
	Game* g,
	sf::Vector3f pos,
	IsoRotation rot,
	TransitRoute route,
	std::weak_ptr<Entity> office) {
	return Entity::createEntity(
		g,
		EntityTag::CargoTruck,
		new Transform(pos, rot),
		new IsoSpriteRenderer(
			ResourceLoader::get()->getSprite("vehicles/vehicles", "cargoTruck_NE.png", true),
			ResourceLoader::get()->getSprite("vehicles/vehicles", "cargoTruck_SE.png", true),
			ResourceLoader::get()->getSprite("vehicles/vehicles", "cargoTruck_SW.png", true),
			ResourceLoader::get()->getSprite("vehicles/vehicles", "cargoTruck_NW.png", true),
			sf::Vector3f(0.5f, 0.5f, 0)
		),
		new CargoVehicleController(route, office, TransitStop::TransitType::Car, g->getTime()),
		nullptr,
		new MailContainer(),
		nullptr,
		new RoadPathfinder()
	);
}

std::shared_ptr<Entity> VehiclePresets::train(
	Game* g,
	sf::Vector3f pos,
	IsoRotation rot,
	TransitRoute route,
	std::weak_ptr<Entity> depot
) {
	return Entity::createEntity(
		g,
		EntityTag::Train,
		new Transform(pos, rot),
		new IsoSpriteRenderer(
			ResourceLoader::get()->getSprite("vehicles/vehicles", "train-N.png", true),
			ResourceLoader::get()->getSprite("vehicles/vehicles", "train-E.png", true),
			ResourceLoader::get()->getSprite("vehicles/vehicles", "train-W.png", true),
			ResourceLoader::get()->getSprite("vehicles/vehicles", "train-S.png", true),
			sf::Vector3f(0.5f, 0.5f, 0)
		),
		new CargoVehicleController(route, depot, TransitStop::TransitType::Train, g->getTime()),
		nullptr,
		new MailContainer(),
		nullptr,
		new RailsPathfinder()
	);
}
std::shared_ptr<Entity> VehiclePresets::plane(
	Game* g,
	sf::Vector3f pos,
	IsoRotation rot,
	TransitRoute route,
	std::weak_ptr<Entity> depot) {
	return Entity::createEntity(
		g,
		EntityTag::Airplane,
		new Transform(pos, rot),
		new IsoSpriteRenderer(
			ResourceLoader::get()->getSprite("vehicles/vehicles", "plane-N", true),
			ResourceLoader::get()->getSprite("vehicles/vehicles", "plane-E", true),
			ResourceLoader::get()->getSprite("vehicles/vehicles", "plane-W", true),
			ResourceLoader::get()->getSprite("vehicles/vehicles", "plane-S", true),
			sf::Vector3f(0.5f, 0.5f, 0)
		),
		new CargoVehicleController(route, depot, TransitStop::TransitType::Airplane, g->getTime()),
		nullptr,
		new MailContainer(),
		nullptr,
		new AirPathfinder()
	);
}
