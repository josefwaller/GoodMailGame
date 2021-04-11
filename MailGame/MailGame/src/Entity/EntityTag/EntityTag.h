#pragma once
#include <string>
#include "Entity/EntityPresets/BuildingPresets/BuildingPresets.h"
#include "Entity/EntityPresets/VehiclePresets/VehiclePresets.h"
#include <map>
#include <functional>
#include <memory>
#include "System/IsoRotation/IsoRotation.h"
#include <SFML/Graphics/Sprite.hpp>

/*
 * This is the big boi macro that is used for doing meta stuff with entities
 * The first parameter is the name of the entity
 * It is used to generate the EntityTag enum, the conversion to string method,
 * and the conversion from string method
 * The second parameter is the method to construct one of these entities from
 * 3 given parameters: Game* g, sf::Vector2f pos, and IsoRotation rot
 * It is used when loading to initialize all the entities before setting up components
 */
#define ENTITY_TAGS \
	F(Residence, BuildingPresets::residence(g, pos, rot)) \
	F(PostOffice, BuildingPresets::postOffice(g, pos, rot)) \
	F(MailBox, BuildingPresets::mailBox(g, pos, rot)) \
	F(MailTruck, VehiclePresets::mailTruck(g, pos, rot, MailTruckRoute(false, 0), {}, 0)) \
	F(CargoTruckDepot, BuildingPresets::cargoTruckDepot(g, pos, rot)) \
	F(CargoTruck, VehiclePresets::cargoTruck(g, pos, rot, TransitRoute(0, VehicleModel::SlowCargoTruck), {}, 0)) \
	F(TrainStation, BuildingPresets::trainStation(g, pos, rot)) \
	F(Train, VehiclePresets::train(g, pos, rot, TransitRoute(0, VehicleModel::SlowTrain), {}, 0)) \
	F(TrainCar, VehiclePresets::trainCar(g, pos, rot, IsoSprite(sf::Sprite(), sf::Sprite(), sf::Sprite(), sf::Sprite()))) \
	F(Airport, BuildingPresets::airport(g, pos, rot)) \
	F(Airplane, VehiclePresets::plane(g, pos, rot, TransitRoute(0, VehicleModel::SlowAirplane), {}, 0))\
	F(RailwaySwitch, BuildingPresets::railwaySwitch(g, pos, rot))\
	F(Warehouse, BuildingPresets::warehouse(g, pos, rot))\
	F(CarDock, BuildingPresets::carDock(g, pos, rot))\
	F(TrainDock, BuildingPresets::trainDock(g, pos, rot))\
	F(AirplaneDock, BuildingPresets::airplaneDock(g, pos, rot))

#define F(x, y) x,
enum class EntityTag {
	ENTITY_TAGS
};

#undef F
#define F(x, y) { EntityTag::x, #x },

const std::map<EntityTag, std::string> tagStrs = {
	ENTITY_TAGS
};

std::string entityTagToString(EntityTag t);

#undef F
#define F(x, y) { #x, EntityTag::x },

const std::map<std::string, EntityTag> strTags = {
	ENTITY_TAGS
};
EntityTag strToEntityTag(std::string s);

#undef F
#define F(x, y) { EntityTag::x, [&](Game* g, sf::Vector3f pos, IsoRotation rot) { return y; }},

const std::map<EntityTag, std::function<std::shared_ptr<Entity>(Game*, sf::Vector3f, IsoRotation rot)>> loadEntityFuncs = {
	ENTITY_TAGS
};

std::shared_ptr<Entity> entityTagToEntity(EntityTag tag, Game* g, sf::Vector3f pos, IsoRotation rot);

#undef F
#undef ENTITY_TAGS
