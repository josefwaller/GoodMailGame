#include "ResidenceController.h"
#include "Entity/Entity.h"
#include "Component/MailContainer/MailContainer.h"
#include "Component/ComponentType/ComponentType.h"
#include "Component/Transform/Transform.h"
#include "Mail/Mail.h"
#include "Game/Game.h"
#include "System/Utils/Utils.h"
#include <imgui.h>
#include "System/SaveData/SaveData.h"

#include <stdexcept>

ResidenceController::ResidenceController(gtime_t time) {}

void ResidenceController::init() {
	std::vector<sf::Vector2i> residenceLocations = this->getEntity()->getGameMap()->getResidences();
	id_t cityId = this->getEntity()->getGameMap()->getTileAt(Utils::toVector2i(this->getEntity()->transform->getPosition())).cityId;
	for (size_t i = 0; i < 3; i++) {
		sf::Vector2i res;
		while (true) {
			size_t pos = rand() % residenceLocations.size();
			res = residenceLocations.at(pos);
			Tile tile = this->getEntity()->getGameMap()->getTileAt(res);
			// Don't add yourself as a destination
			if (tile.building.lock() != this->getEntity()) {
				// Add one from the same city and two from different cities
		//		if ((i == 0 && tile.cityId == cityId) || (i != 0 && tile.cityId != cityId)) {
				break;
				//		}
			}
		}
		this->destinations.push_back({ rand() % 24, res });
	}
}

void ResidenceController::onHourChange(hour_t hour) {
	for (auto p : this->destinations) {
		if (p.first == hour) {
			this->generateLetter(p.second, this->getEntity()->getGame()->getMidnightTime() + Game::UNITS_IN_GAME_HOUR * hour);
		}
	}
}

void ResidenceController::generateLetter(sf::Vector2i dest, gtime_t departTime) {
	// Create a new letter addressed to self (tba)
	sf::Vector3f pos = this->getEntity()->transform->getPosition();
	sf::Vector2i src(Utils::toVector2i(pos));
	Mail letter(src, dest, this->getEntity()->getGame()->getTime());
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
	using namespace SaveKeys;
	SaveData sd(componentTypeToStr(ComponentType::Controller));
	for (auto p : this->destinations) {
		SaveData d(DEST);
		d.addVector2i(DEST, p.second);
		d.addSizeT(HOUR, p.first);
		sd.addData(d);
	}
	return sd;
}
void ResidenceController::fromSaveData(SaveData data) {
	using namespace SaveKeys;
	for (SaveData d : data.getDatas()) {
		this->destinations.push_back(
			{
			d.getSizeT(HOUR),
			d.getVector2i(DEST)
			}
		);
	}
}

std::vector<sf::Vector2i> ResidenceController::getDestinations() {
	std::vector<sf::Vector2i> toReturn;
	for (auto kv : this->destinations) {
		toReturn.push_back(kv.second);
	}
	return toReturn;
}