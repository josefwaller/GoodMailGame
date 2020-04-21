#include "MailTruckController.h"
#include "Game/Game.h"
#include <stdexcept>
#include "Entity/Entity.h"
#include "Component/TransitStop/TransitStop.h"
#include "Component/MailContainer/MailContainer.h"
#include "Component/ComponentType/ComponentType.h"
#include "Component/Transform/Transform.h"
#include "Component/Pathfinder/Pathfinder.h"
#include "System/SaveData/SaveData.h"
#include "Mail/Mail.h"
#include <imgui.h>

const float MailTruckController::SPEED = 1.0f;

// Set stop to -1 to avoid skipping the first stop
MailTruckController::MailTruckController(MailTruckRoute r, std::weak_ptr<Entity> o)
	: route(r), office(o), hasPickedUpMail(false) {
	// Build and set the stops
	std::vector<sf::Vector2f> stops;
	for (MailTruckRouteStop s : this->route.stops) {
		stops.push_back(sf::Vector2f(s.target.value()));
	}
	stops.push_back(
		o.lock()->transitStop->getTransitLocation()
	);
	this->setStops(stops);
}

void MailTruckController::update(float delta) {
	if (!hasPickedUpMail && this->route.isDelivering) {
		hasPickedUpMail = true;
		this->pickupMailFromOffice();
	}
	VehicleController::update(delta);
	// Draw GUI
	ImGui::Begin("Mail Trucks");
	char buf[200];
	sprintf_s(buf, "Truck %zu has %zu letters",
		this->getEntity()->getId(),
		this->getEntity()->mailContainer->getNumLetters()
	);
	ImGui::Text(buf);
	ImGui::End();

}
void MailTruckController::dropOffMail(sf::Vector2i pos) {
	for (int x = -1; x < 2; x++) {
		for (int y = -1; y < 2; y++) {
			if ((x == 0 || y == 0) && x != y) {
				// Go through mail
				for (Mail l : this->getEntity()->mailContainer->getMail()) {
					if (l.getDest() == pos + sf::Vector2i(x, y)) {
						// Deliver the letter
						this->getEntity()->mailContainer->removeMail({ l });
					}
				}
			}
		}
	}
}


void MailTruckController::onArriveAtTile(sf::Vector2f point) {
	// Pick up letters if a mailbox is on the point
	if (!this->route.isDelivering) {
		auto entities = this->getEntity()->getGame()->getEntities();
		for (auto it = entities.begin(); it != entities.end(); it++) {
			if ((*it)->tag == EntityTag::MailBox && (*it)->transitStop->getTransitLocation() == point) {
				// Take all the letters
				(*it)->mailContainer->transferAllMailTo(this->getEntity()->mailContainer);
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
void MailTruckController::onArriveAtDest() {
	// If we were going back to the office, just delete yourself
	// Give all the letters back to the post office
	if (auto o = this->office.lock()) {
		this->getEntity()->mailContainer->transferAllMailTo(o->mailContainer);
	}
	// Delete self
	this->getEntity()->getGame()->removeEntity(this->getEntity());
}
float MailTruckController::getSpeed() {
	return SPEED;
}
void MailTruckController::pickupMailFromOffice() {
	// Get the position of the post office
	sf::Vector2f pos(this->office.lock()->transitStop->getTransitLocation());
	// Get all the stops along the route (i.e. from post office -> through all the stops -> back to post office)
	std::vector<sf::Vector2f> allStops = { pos };
	for (MailTruckRouteStop s : route.stops) {
		allStops.push_back(sf::Vector2f(s.target.value()));
	}
	allStops.push_back(pos);
	// Now gather all the tiles between the stops
	std::vector<sf::Vector2i> allPoints;
	for (size_t i = 0; i < allStops.size() - 1; i++) {
		// Get the points
		std::vector<sf::Vector2f> pointsBetween = this->getEntity()->pathfinder->findPathBetweenPoints(
			allStops[i],
			allStops[i + 1]
		);
		// Add to all points
		for (auto it = pointsBetween.begin(); it != pointsBetween.end(); it++) {
			allPoints.push_back(sf::Vector2i(*it));
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
			if (!this->getEntity()->getGame()->getGameMap()->hasRoadAt(p2.x, p2.y))
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
	// Take the mail
	this->office.lock()->mailContainer->transferSomeMailTo(mailForRoute, this->getEntity()->mailContainer);
}

std::optional<SaveData> MailTruckController::getSaveData() {
	SaveData sd(componentTypeToStr(ComponentType::Controller));
	if (this->office.lock()) {
		sd.addValue("OfficeId", this->office.lock()->getId());
	}
	sd.addData(mailTruckRouteToSaveData(this->route));
	return sd;
}
