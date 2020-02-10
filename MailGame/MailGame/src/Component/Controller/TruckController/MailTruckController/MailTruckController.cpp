#include "MailTruckController.h"
#include "Game/Game.h"
#include <stdexcept>
#include "Entity/Entity.h"
#include "Component/Transform/Transform.h"
#include "Component/MailContainer/MailContainer.h"
#include "GameMap/GameMap.h"
#include "Mail/Mail.h"
#include <imgui.h>

const float MailTruckController::SPEED = 1.0f;

// Set stop to -1 to avoid skipping the first stop
MailTruckController::MailTruckController(MailTruckRoute r, std::weak_ptr<Entity> o)
	: route(r), office(o) {
	// Build and set the stops
	std::vector<sf::Vector2i> stops;
	for (MailTruckRouteStop s : this->route.stops) {
		stops.push_back(s.target.value());
	}
	stops.push_back(sf::Vector2i(
		o.lock()->transform->getPosition() + o.lock()->transform->getRotation().getUnitVector()
	));
	this->setStops(stops);
}

void MailTruckController::update(float delta) {
	TruckController::update(delta);
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
			if ((*it)->tag == EntityTag::MailBox && (*it)->transform->getPosition() == point) {
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
