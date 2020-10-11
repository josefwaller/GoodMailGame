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
MailTruckController::MailTruckController(MailTruckRoute r, std::weak_ptr<Entity> o, gtime_t departTime)
	: route(r), office(o), hasPickedUpMail(false), VehicleController(departTime) {
	setRouteStops();
}
void MailTruckController::setRouteStops() {
	std::vector<VehicleControllerStop> vStops;
	for (MailTruckRouteStop stop : this->route.stops) {
		// TODO: Clean this up too
		if (!stop.target.has_value())
			continue;

		vStops.push_back(VehicleControllerStop(stop.points.back().pos, stop.points.back().expectedTime, stop.points));
	}
	this->setStops(vStops);
	// Build and set the stops
}

void MailTruckController::update(float delta) {
	if (!hasPickedUpMail && this->route.isDelivering) {
		hasPickedUpMail = true;
		this->pickupMailFromOffice();
	}
	VehicleController::update(delta);
	// Draw GUI
	char buf[200];
	sprintf_s(buf, "Mail Truck %llu", this->getEntity()->getId());
	ImGui::Begin(buf);
	sprintf_s(buf, "%zu letters",
		this->getEntity()->mailContainer->getNumLetters()
	);
	ImGui::Text(buf);
	sf::Vector3f pos = this->getEntity()->transform->getPosition();
	sprintf_s(buf, "Position (%f, %f, %f)", pos.x, pos.y, pos.z);
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
						l.onDelivery(this->getEntity()->getGame()->getTime());
						this->getEntity()->mailContainer->removeMail({ l });
					}
				}
			}
		}
	}
}


void MailTruckController::onArriveAtTile(sf::Vector2i point) {
	// Pick up letters if a mailbox is on the point
	if (!this->route.isDelivering) {
		auto entities = this->getEntity()->getGame()->getEntities();
		for (auto it = entities.begin(); it != entities.end(); it++) {
			if ((*it)->tag == EntityTag::MailBox) {
				sf::Vector3f pos = (*it)->transitStop->getCarStop().tile;
				// Check if the transit location is the tile it just arrived at
				if (sf::Vector2f(pos.x, pos.y) == sf::Vector2f(point)) {
					// Take all the letters
					(*it)->mailContainer->transferAllMailTo(this->getEntity()->mailContainer);
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
	for (size_t i = 0; i < route.stops.size(); i++) {
		// Get the points
		std::vector<RoutePoint> pointsBetween = this->route.stops[i].points;
		// Add to all points
		for (auto it = pointsBetween.begin(); it != pointsBetween.end(); it++) {
			allPoints.push_back(sf::Vector2i(it->pos.x, it->pos.y));
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
	// Take the mail
	this->office.lock()->mailContainer->transferSomeMailTo(mailForRoute, this->getEntity()->mailContainer);
}

std::optional<SaveData> MailTruckController::getSaveData() {
	SaveData sd(componentTypeToStr(ComponentType::Controller));
	if (this->office.lock()) {
		sd.addValue("officeId", this->office.lock()->getId());
	}
	sd.addData(mailTruckRouteToSaveData(this->route));
	sd.addValuesFrom(VehicleController::getSaveData().value());
	return sd;
}
void MailTruckController::fromSaveData(SaveData data) {
	if (data.hasValue("officeId")) {
		this->office = this->getEntity()->getGame()->getEntityById(std::stoull(data.getValue("officeId")));
	}
	this->stopIndex = std::stoull(data.getValue("stopIndex"));
	this->route = saveDataToMailTruckRoute(data.getDatas()[0]);
	setRouteStops();
	VehicleController::fromSaveData(data);
}
