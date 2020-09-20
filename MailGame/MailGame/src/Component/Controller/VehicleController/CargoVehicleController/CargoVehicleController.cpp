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
	TransitStop::TransitType type,
	gtime_t departTime
) : depot(d), route(r), type(type), VehicleController(departTime) {
	setRouteStops();
}
void CargoVehicleController::setRouteStops() {
	std::vector<sf::Vector3f> depotPath = {};
	if (depot.lock()) {
		depotPath = getDepartingTransitPath(depot.lock(), this->type);
	}
	// Add all the locations to stops
	std::vector<sf::Vector3f> stops;
	// Add the depot
	// TODO: Separate this into departingDepotPath and enteringDepotPath
	stops.insert(stops.end(), depotPath.begin(), depotPath.end());
	// Add the stops for the route
	for (TransitRouteStop stop : route.stops) {
		if (auto s = stop.target.lock()) {
#ifdef _DEBUG
			if (!s->transitStop) {
				throw std::runtime_error("Cargo truck has stop that does not have TransitStop!");
			}
#endif
			// Add the arriving and departing path
			auto arrivingPath = getArrivingTransitPath(s, this->type);
			stops.insert(stops.end(), arrivingPath.begin(), arrivingPath.end());
			auto departingPath = getDepartingTransitPath(s, this->type);
			stops.insert(stops.end(), departingPath.begin(), departingPath.end());
		}
	}
	// Add the stops going into the depot
	// See Todo above
	if (this->depot.lock())
		depotPath = getArrivingTransitPath(depot.lock(), this->type);
	stops.insert(stops.end(), depotPath.begin(), depotPath.end());
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
	sd.addValuesFrom(VehicleController::getSaveData().value());
	return sd;
}
void CargoVehicleController::fromSaveData(SaveData data) {
	this->route = saveDataToTransitRoute(this->getEntity()->getGame(), data.getData("TransitRoute"));
	if (data.hasValue("depotId")) {
		this->depot = this->getEntity()->getGame()->getEntityById(std::stoull(data.getValue("depotId")));
	}
	this->setRouteStops();
	VehicleController::fromSaveData(data);
}


