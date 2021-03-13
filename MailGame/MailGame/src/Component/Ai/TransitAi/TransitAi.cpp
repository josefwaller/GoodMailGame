#include "TransitAi.h"
#include "Entity/Entity.h"
#include "Entity/EntityTag/EntityTag.h"
#include "Component/TransitStop/TransitStop.h"
#include "Component/MailContainer/MailContainer.h"
#include "Component/ComponentType/ComponentType.h"
#include "Game/Game.h"
#include "System/SaveData/SaveData.h"
#include <stdexcept>
#include <imgui.h>

TransitAi::TransitAi(
	TransitRoute r,
	std::weak_ptr<Entity> d,
	TransitType type
) : depot(d), route(r), type(type) {
}

void TransitAi::renderUi() {
	ImGui::PushID(this->getEntity()->getId());
	char buf[200];
	sprintf_s(buf, "%s %llu", entityTagToString(this->getEntity()->tag).c_str(), this->getEntity()->getId());
	ImGui::Begin(buf);

	ImGui::End();
	ImGui::PopID();
}
std::vector<VehicleControllerStop> TransitAi::getStops() {
	std::vector<VehicleControllerStop> stops;
	VehicleModelInfo modelInfo = VehicleModelInfo::getModelInfo(this->route.model);
	// First add the departing path for the depot
	// Even though a truck should never not have a depot, if loading it may not start with a valid reference
	// So we have to check
	if (this->depot.lock()) {
		// Add the depot as the first stop
		std::shared_ptr<Entity> d = this->depot.lock();
		stops.push_back(VehicleControllerStop(
			d,
			modelInfo.getLoadTime()
		));
	}
	// Add a stop for every stop along the route
	for (auto it = this->route.stops.begin(); it != this->route.stops.end(); it++) {
		// If the stop has a target
		if (it->target.lock()) {
			// Add the arriving path
			stops.push_back(VehicleControllerStop(
				it->target,
				modelInfo.getUnloadTime() + modelInfo.getLoadTime()
			));
		}
	}
	// Go back to the depot
	if (this->depot.lock()) {
		// Add the depot as the first stop
		std::shared_ptr<Entity> d = this->depot.lock();
		stops.push_back(VehicleControllerStop(
			d,
			modelInfo.getUnloadTime()
		));
	}
	return stops;
}
void TransitAi::onArriveAtDest() {
	// Give letters to depot and destory self
	this->getEntity()->mailContainer->transferAllMailTo(this->depot.lock()->mailContainer);
	this->getEntity()->getGame()->removeEntity(this->getEntity());
}
void TransitAi::onArriveAtStop(size_t stopIndex) {
	// Account for stopping at the depot
	stopIndex--;
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

std::optional<SaveData> TransitAi::getSaveData() {
	SaveData sd(componentTypeToStr(ComponentType::Controller));
	sd.addData(this->route.getSaveData());
	if (depot.lock())
		sd.addValue(SaveKeys::DEPOT_ID, depot.lock()->getId());
	return sd;
}
void TransitAi::fromSaveData(SaveData data) {
	this->route = TransitRoute(data.getData(SaveKeys::TRANSIT_ROUTE), this->getEntity()->getGame());
	if (data.hasValue(SaveKeys::DEPOT_ID)) {
		this->depot = this->getEntity()->getGame()->getEntityById(data.getSizeT(SaveKeys::DEPOT_ID));
	}
}