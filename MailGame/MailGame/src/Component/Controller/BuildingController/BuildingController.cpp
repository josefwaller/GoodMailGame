#include "BuildingController.h"
#include "Entity/Entity.h"
#include "Component/MailContainer/MailContainer.h"
#include "Component/Transform/Transform.h"
#include "Mail/Letter/Letter.h"
#include "Game/Game.h"
#include <imgui.h>

#include <stdexcept>

const float BuildingController::GENERATE_INTERVAL = 5000.0f;

void BuildingController::update(float time) {
	if (clk.getElapsedTime().asMilliseconds() >= GENERATE_INTERVAL) {
		clk.restart();
		this->generateLetter();
	}
}

void BuildingController::generateLetter() {
	// Create a new letter addressed to self (tba)
	sf::Vector2i pos(this->getEntity()->transform->getPosition());
	Letter l(pos, pos);
	// Check if there is a mailbox close enough to get it
	auto entities = this->getEntity()->getGame()->getEntities();
	for (auto it = entities.begin(); it != entities.end(); it++) {
		// Check if a mailbox
		if ((*it)->tag == EntityTag::MailBox) {
			// Check if it's close enough
			sf::Vector2f oPos = (*it)->transform->getPosition();
			sf::Vector2f pos = this->getEntity()->transform->getPosition();
			if (pow(oPos.x - pos.x, 2) + pow(oPos.y - pos.y, 2) <= pow(5.0f, 2)) {
				// Give the letter to it
#ifdef _DEBUG
				if ((*it)->mailContainer == nullptr) {
					throw std::runtime_error("Mailbox does not have mailContainer");
				}
#endif
				(*it)->mailContainer->addMail({ l });
			}
		}
	}
}
