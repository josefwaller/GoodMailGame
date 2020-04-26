#include "ResidenceController.h"
#include "Entity/Entity.h"
#include "Component/MailContainer/MailContainer.h"
#include "Component/ComponentType/ComponentType.h"
#include "Component/Transform/Transform.h"
#include "Mail/Mail.h"
#include "Game/Game.h"
#include <imgui.h>
#include "System/SaveData/SaveData.h"

#include <stdexcept>

const float ResidenceController::GENERATE_INTERVAL = 5000.0f;

void ResidenceController::update(float time) {
	if (clk.getElapsedTime().asMilliseconds() >= GENERATE_INTERVAL) {
		clk.restart();
		this->generateLetter();
	}
}

void ResidenceController::generateLetter() {
	// Create a new letter addressed to self (tba)
	sf::Vector3i pos(this->getEntity()->transform->getPosition());
	sf::Vector2i p(pos.x, pos.y);
	Mail letter(p, p);
	// Check if there is a mailbox close enough to get it
	auto entities = this->getEntity()->getGame()->getEntities();
	for (auto it = entities.begin(); it != entities.end(); it++) {
		// Check if a mailbox
		if ((*it)->tag == EntityTag::MailBox) {
			// Check if it's close enough
			sf::Vector3f oPos = (*it)->transform->getPosition();
			if (pow(oPos.x - pos.x, 2) + pow(oPos.y - pos.y, 2) <= pow(5.0f, 2)) {
				// Give the letter to it
#ifdef _DEBUG
				if ((*it)->mailContainer == nullptr) {
					throw std::runtime_error("Mailbox does not have mailContainer");
				}
#endif
				(*it)->mailContainer->addMail({ letter });
			}
		}
	}
}

std::optional<SaveData> ResidenceController::getSaveData() {
	SaveData sd(componentTypeToStr(ComponentType::Controller));
	return sd;
}
