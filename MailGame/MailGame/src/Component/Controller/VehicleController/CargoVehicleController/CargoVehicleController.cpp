#include "CargoVehicleController.h"
#include "Entity/Entity.h"
#include "Component/TransitStop/TransitStop.h"
#include "Component/MailContainer/MailContainer.h"
#include "Component/ComponentType/ComponentType.h"
#include "Game/Game.h"
#include "System/SaveData/SaveData.h"
#include <stdexcept>

CargoVehicleController::CargoVehicleController(
	TransitRoute r,
	std::weak_ptr<Entity> d,
	TransitStop::TransitType type
) : depot(d), route(r), type(type) {
	setRouteStops();
}
void CargoVehicleController::setRouteStops() {
	// Add all the locations to stops
	std::vector<sf::Vector3f> stops;
	for (TransitRouteStop stop : route.stops) {
		if (auto s = stop.target.lock()) {
#ifdef _DEBUG
			if (!s->transitStop) {
				throw std::runtime_error("Cargo truck has stop that does not have TransitStop!");
			}
#endif
			auto path = getTransitPos(s);
			stops.insert(stops.end(), path.begin(), path.end());
		}
	}
	if (depot.lock()) {
		auto path = getTransitPos(depot.lock());
		stops.insert(stops.end(), path.begin(), path.end());
	}
	this->setStops(stops);
}
void CargoVehicleController::onArriveAtDest() {
	// Give letters to depot and destory self
	this->getEntity()->mailContainer->transferAllMailTo(this->depot.lock()->mailContainer);
	this->getEntity()->getGame()->removeEntity(this->getEntity());
}
void CargoVehicleController::onArriveAtStop(size_t stopIndex) {
	// For the last stop (i.e. the depot) we don't need to transfer any letters
	if (stopIndex >= this->route.stops.size())
		return;
	TransitRouteStop stop = this->route.stops[stopIndex];
	if (!stop.target.lock())
		return;
	// Get game map
	GameMap* gMap = this->getEntity()->getGame()->getGameMap();
	// Get mail containers
	auto mail = this->getEntity()->mailContainer;
	auto otherMail = stop.target.lock()->mailContainer;
	// What mail to pick up and drop off
	std::vector<Mail> toDropOff;
	std::vector<Mail> toPickUp;
	// Get all the mail to drop off
	for (auto m : mail->getMail()) {
		// Get the tile destintation
		sf::Vector2i d = m.getDest();
		Tile dest = gMap->getTileAt((size_t)d.x, (size_t)d.y);
		// Check if the truck should drop off the letter
		if (std::find(stop.toDropOff.begin(), stop.toDropOff.end(), dest.postalCode) != stop.toDropOff.end()) {
			toDropOff.push_back(m);
		}
	}
	// Get all mail to pick up
	for (auto m : otherMail->getMail()) {
		// Get the tile
		sf::Vector2i d = m.getDest();
		Tile dest = gMap->getTileAt((size_t)d.x, (size_t)d.y);
		// Check if the truck should pick it up
		if (std::find(stop.toPickUp.begin(), stop.toPickUp.end(), dest.postalCode) != stop.toPickUp.end()) {
			toPickUp.push_back(m);
		}
	}
	// Transfer letters between
	mail->transferSomeMailTo(toDropOff, otherMail);
	otherMail->transferSomeMailTo(toPickUp, mail);
}
float CargoVehicleController::getSpeed() { return 1.0f; };

std::optional<SaveData> CargoVehicleController::getSaveData() {
	SaveData sd(componentTypeToStr(ComponentType::Controller));
	sd.addData(transitRouteToSaveData(this->route));
	if (depot.lock())
		sd.addValue("depotId", depot.lock()->getId());
	sd.addValue("stopIndex", this->stopIndex);
	return sd;
}
void CargoVehicleController::fromSaveData(SaveData data) {
	this->route = saveDataToTransitRoute(this->getEntity()->getGame(), data.getData("TransitRoute"));
	if (data.hasValue("depotId")) {
		this->depot = this->getEntity()->getGame()->getEntityById(std::stoull(data.getValue("depotId")));
	}
	this->stopIndex = std::stoull(data.getValue("stopIndex"));
	this->setRouteStops();
}

std::vector<sf::Vector3f> CargoVehicleController::getTransitPos(std::shared_ptr<Entity> e) {
	auto transit = e->transitStop;
	switch (this->type) {
	case TransitStop::TransitType::Car:
		return { transit->getCarStop().tile };
	case TransitStop::TransitType::Train:
		return { transit->getTrainStop().tile };
	case TransitStop::TransitType::Airplane:
		// Compute where we need to go
		auto aStop = transit->getAirplaneStop();
		auto dir = aStop.end - aStop.begin;
		// TBA add z
		sf::Vector3f unit = dir / (sqrt(dir.x * dir.x + dir.y * dir.y));
		// Th horizontal distance to cover when descending
		const float DESCENT_LENGTH = 5.0f;
		const float PLANE_HEIGHT = 4.0f;
		sf::Vector3f airStart = aStop.begin - DESCENT_LENGTH * unit;
		airStart.z = PLANE_HEIGHT;
		sf::Vector3f airEnd = aStop.end + unit * DESCENT_LENGTH;
		airEnd.z = PLANE_HEIGHT;
		return  { airStart, aStop.begin, aStop.end, airEnd };
	}
	return {};
}
