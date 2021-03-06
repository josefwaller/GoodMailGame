#include "VehiclePresets.h"
#include "Entity/Entity.h"
#include "ResourceLoader/ResourceLoader.h"
// Components
// Transform
#include "Component/Transform/Transform.h"
// Renderers
#include "Component/Renderer/IsoSpriteRenderer/IsoSpriteRenderer.h"
// Controllers
#include "Component/Controller/VehicleController/CarController/CarController.h"
#include "Component/Controller/VehicleController/TrainController/TrainController.h"
#include "Component/Controller/VehicleController/PlaneController/PlaneController.h"
// Mail Container
#include "Component/MailContainer/MailContainer.h"
// Pathfinders
#include "Component/Pathfinder/RoadPathfinder/RoadPathfinder.h"
#include "Component/Pathfinder/RailsPathfinder/RailsPathfinder.h"
#include "Component/Pathfinder/AirPathfinder/AirPathfinder.h"
#include "Game/Game.h"
// Ai
#include "Component/Ai/PickupDeliveryAi/PickupDeliveryAi.h"
#include "Component/Ai/TransitAi/TransitAi.h"

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
			IsoSprite(
				ResourceLoader::get()->getSprite("vehicles/vehicles", "mailTruck-N", true),
				ResourceLoader::get()->getSprite("vehicles/vehicles", "mailTruck-E", true),
				ResourceLoader::get()->getSprite("vehicles/vehicles", "mailTruck-S", true),
				ResourceLoader::get()->getSprite("vehicles/vehicles", "mailTruck-W", true)
			),
			sf::Vector3f(0.5f, 0.5f, 0)
		),
		new CarController(g->getTime(), route.model, {}),
		nullptr,
		new MailContainer(),
		nullptr,
		new RoadPathfinder(),
		new PickupDeliveryAi(route, postOffice)
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
			IsoSprite(
				ResourceLoader::get()->getSprite("vehicles/vehicles", "cargoTruck_NE.png", true),
				ResourceLoader::get()->getSprite("vehicles/vehicles", "cargoTruck_SE.png", true),
				ResourceLoader::get()->getSprite("vehicles/vehicles", "cargoTruck_SW.png", true),
				ResourceLoader::get()->getSprite("vehicles/vehicles", "cargoTruck_NW.png", true)
			),
			sf::Vector3f(0.5f, 0.5f, 0)
		),
		new CarController(g->getTime(), route.model),
		nullptr,
		new MailContainer(),
		nullptr,
		new RoadPathfinder(),
		new TransitAi(route, office, TransitType::Car)
	);
}

std::shared_ptr<Entity> VehiclePresets::train(
	Game* g,
	sf::Vector3f pos,
	IsoRotation rot,
	TransitRoute route,
	std::weak_ptr<Entity> depot
) {
	std::vector<std::weak_ptr<Entity>> trainCars;
	if (route.cargoCarModel.has_value()) {
		CargoCarInfo cInfo = CargoCarInfo::get(route.cargoCarModel.value());
		for (size_t i = 0; i < route.numCargoCars; i++) {
			auto car = VehiclePresets::trainCar(g, pos, rot, cInfo.getSprites());
			g->addEntity(car);
			trainCars.push_back(car);
		}
	}
	return Entity::createEntity(
		g,
		EntityTag::Train,
		new Transform(pos, rot),
		new IsoSpriteRenderer(
			IsoSprite(
				ResourceLoader::get()->getSprite("vehicles/vehicles", "train-N.png", true),
				ResourceLoader::get()->getSprite("vehicles/vehicles", "train-E.png", true),
				ResourceLoader::get()->getSprite("vehicles/vehicles", "train-W.png", true),
				ResourceLoader::get()->getSprite("vehicles/vehicles", "train-S.png", true)
			),
			sf::Vector3f(0.5f, 0.5f, 0)
		),
		new TrainController(g->getTime(), route.model, trainCars),
		nullptr,
		new MailContainer(),
		nullptr,
		new RailsPathfinder(),
		new TransitAi(route, depot, TransitType::Train)
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
			IsoSprite(
				ResourceLoader::get()->getSprite("vehicles/vehicles", "plane-N", true),
				ResourceLoader::get()->getSprite("vehicles/vehicles", "plane-E", true),
				ResourceLoader::get()->getSprite("vehicles/vehicles", "plane-W", true),
				ResourceLoader::get()->getSprite("vehicles/vehicles", "plane-S", true)
			),
			sf::Vector3f(0.5f, 0.5f, 0)
		),
		new PlaneController(g->getTime(), route.model),
		nullptr,
		new MailContainer(),
		nullptr,
		new AirPathfinder(),
		new TransitAi(route, depot, TransitType::Airplane)
	);
}
std::shared_ptr<Entity> VehiclePresets::trainCar(Game* g, sf::Vector3f pos, IsoRotation rot, IsoSprite sprites) {
	return Entity::createEntity(
		g,
		EntityTag::TrainCar,
		new Transform(pos, rot),
		new IsoSpriteRenderer(
			sprites,
			sf::Vector3f(0.5f, 0.5f, 0)
		)
	);
}