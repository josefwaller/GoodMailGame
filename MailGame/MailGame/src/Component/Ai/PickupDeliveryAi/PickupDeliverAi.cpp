#include "PickupDeliveryAi.h"
#include "Game/Game.h"
#include <stdexcept>
#include "Entity/Entity.h"
#include "Component/TransitStop/TransitStop.h"
#include "Component/MailContainer/MailContainer.h"
#include "Component/ComponentType/ComponentType.h"
#include "Component/Transform/Transform.h"
#include "Component/Pathfinder/Pathfinder.h"
#include "System/SaveData/SaveData.h"
#include "System/TransitType/TransitType.h"
#include "Mail/Mail.h"
#include <imgui.h>

const float PickupDeliveryAi::SPEED = 1.0f;

// Set stop to -1 to avoid skipping the first stop
PickupDeliveryAi::PickupDeliveryAi(MailTruckRoute r, std::weak_ptr<Entity> o)
	: route(r), office(o), hasPickedUpMail(false) {
}
std::vector<VehicleControllerStop> PickupDeliveryAi::getStops() {
	// This is messy as hell, but oh well
	if (this->route.isDelivering && !this->hasPickedUpMail) {
		this->hasPickedUpMail = true;
		pickupMailFromOffice();
	}
	std::vector<VehicleControllerStop> stops;
	VehicleModelInfo modelInfo = VehicleModelInfo::getModelInfo(VehicleModel::MailTruck);
	// First add the departing path for the depot
	// Even though a truck should never not have a depot, if loading it may not start with a valid reference
	// So we have to check
	if (this->office.lock()) {
		// Add the depot as the first stop
		std::shared_ptr<Entity> d = this->office.lock();
		stops.push_back(VehicleControllerStop(
			d
		));
	}
	// Add a stop for every stop along the route
	for (auto it = this->route.stops.begin(); it != this->route.stops.end(); it++) {
		// If the stop has a target
		if (it->target.has_value()) {
			// Add the arriving path
			sf::Vector2i pos = it->target.value();
			stops.push_back(VehicleControllerStop(pos));
		}
	}
	// Go back to the depot
	if (this->office.lock()) {
		// Add the depot as the first stop
		std::shared_ptr<Entity> d = this->office.lock();
		stops.push_back(VehicleControllerStop(
			d
		));
	}
	return stops;
}

void PickupDeliveryAi::renderUi() {
	ImGui::PushID(this->getEntity()->getId());
	// Draw GUI
	char buf[200];
	sprintf_s(buf, "%u / %u mail objects",
		this->getEntity()->mailContainer->getNumLetters(), this->getCapacity(this->route)
	);
	ImGui::Text(buf);
	ImGui::PopID();
}
void PickupDeliveryAi::dropOffMail(sf::Vector2i pos) {
	for (int x = -1; x < 2; x++) {
		for (int y = -1; y < 2; y++) {
			if ((x == 0 || y == 0) && x != y) {
				// Go through mail
				for (Mail l : this->getEntity()->mailContainer->getMail()) {
					if (l.getDest() == pos + sf::Vector2i(x, y)) {
						// Deliver the letter
						l.onDelivery(this->getEntity()->getGame()->getTime(), this->getEntity()->getGame());
						this->getEntity()->mailContainer->removeMail({ l });
					}
				}
			}
		}
	}
}

void PickupDeliveryAi::onArriveAtTile(sf::Vector2i point) {
	// Pick up letters if a mailbox is on the point
	if (!this->route.isDelivering) {
		auto entities = this->getEntity()->getGame()->getEntities();
		for (auto it = entities.begin(); it != entities.end(); it++) {
			if ((*it)->tag == EntityTag::MailBox) {
				sf::Vector3f pos = (*it)->transitStop->getCarStop().tile;
				// Check if the transit location is the tile it just arrived at
				if (sf::Vector2f(pos.x, pos.y) == sf::Vector2f(point)) {
					// Take all the letters we can carry
					std::vector<Mail> mail = (*it)->mailContainer->getMail();
					unsigned int numMail = this->getEntity()->mailContainer->getNumLetters();
					mail.resize(std::min(mail.size(), this->getCapacity(this->route) - numMail), Mail(sf::Vector2i(), sf::Vector2i(), 0));
					(*it)->mailContainer->transferSomeMailTo(mail, this->getEntity()->mailContainer, MailEvent::OnMailTruck);
				}
			}
		}
	}
	else {
		// Check for dropping off mail
		if (this->route.isDelivering) {
			this->dropOffMail(sf::Vector2i(point));
		}
	}
}
// When done, the truck should be back at the post office
void PickupDeliveryAi::onArriveAtDest() {
	// If we were going back to the office, just delete yourself
	// Give all the letters back to the post office
	if (auto o = this->office.lock()) {
		this->getEntity()->mailContainer->transferAllMailTo(o->mailContainer);
	}
	// Delete self
	this->getEntity()->getGame()->removeEntity(this->getEntity());
}
void PickupDeliveryAi::pickupMailFromOffice() {
	using Segment = Pathfinder::RoadSegment;
	// Get the position of the post office
	TransitStop::CarStop carStop = this->office.lock()->transitStop->getCarStop();
	sf::Vector3f pos3D(carStop.tile + carStop.dir.value().getUnitVector3D());
	sf::Vector2f pos(pos3D.x, pos3D.y);
	// Get all the stops along the route (i.e. from post office -> through all the stops -> back to post office)
	std::vector<sf::Vector2f> allStops = { pos };
	for (MailTruckRouteStop s : route.stops) {
		if (s.target.has_value())
			allStops.push_back(sf::Vector2f(s.target.value()));
	}
	allStops.push_back(pos);
	// Now gather all the tiles between the stops
	std::vector<sf::Vector2i> allPoints;
	sf::Vector2i prevPoint = route.stops[0].target.value();
	VehicleModelInfo mInfo = VehicleModelInfo::getModelInfo(this->route.model);
	for (size_t i = 0; i < route.stops.size(); i++) {
		if (!route.stops[i].target.has_value())
			continue;
		// Get the points
		sf::Vector2i nextPoint = route.stops[i].target.value();
		std::vector<Segment> pointsBetween = Pathfinder::findCarPath(this->getEntity()->getGameMap(), prevPoint, nextPoint);
		prevPoint = nextPoint;
		// Add to all points
		for (auto it = pointsBetween.begin(); it != pointsBetween.end(); it++) {
			if (it->getType() == Segment::Type::Tile) {
				allPoints.push_back(it->getTile());
			}
		}
	}
	// Add all the points beside these points
	std::vector<sf::Vector2i> pointsAround;
	for (sf::Vector2i p : allPoints) {
		std::vector<sf::Vector2i> pointsToAdd = {
			p + sf::Vector2i(1, 0),
			p + sf::Vector2i(-1, 0),
			p + sf::Vector2i(0, 1),
			p + sf::Vector2i(0, -1)
		};
		for (sf::Vector2i p2 : pointsToAdd) {
			// Add the point if it is not a road
			if (this->getEntity()->getGame()->getGameMap()->hasRoadAt(p2.x, p2.y))
				continue;
			if (std::find(pointsAround.begin(), pointsAround.end(), p2) == pointsAround.end()) {
				pointsAround.push_back(p2);
			}
		}
	}
	// Now go through all the mail and select the ones that are delivered on these points
	std::vector<Mail> mailForRoute;
	for (Mail m : this->office.lock()->mailContainer->getMail()) {
		if (std::find(pointsAround.begin(), pointsAround.end(), m.getDest()) != pointsAround.end()) {
			mailForRoute.push_back(m);
		}
	}
	// Don't pick up more mail than you can carry
	mailForRoute.resize(std::min(mailForRoute.size(), this->getCapacity(this->route)), Mail(sf::Vector2i(), sf::Vector2i(), 0));
	// Take the mail
	this->office.lock()->mailContainer->transferSomeMailTo(mailForRoute, this->getEntity()->mailContainer, MailEvent::OnMailTruck);
}

std::optional<SaveData> PickupDeliveryAi::getSaveData() {
	SaveData sd(componentTypeToStr(ComponentType::Ai));
	if (this->office.lock()) {
		sd.addSizeT(SaveKeys::OFFICE_ID, this->office.lock()->getId());
	}
	sd.addData(this->route.getSaveData());
	return sd;
}
void PickupDeliveryAi::fromSaveData(SaveData data) {
	if (data.hasValue(SaveKeys::OFFICE_ID)) {
		this->office = this->getEntity()->getGame()->getEntityById(data.getSizeT(SaveKeys::OFFICE_ID));
	}
	this->route = MailTruckRoute(data.getDatas()[0]);
}