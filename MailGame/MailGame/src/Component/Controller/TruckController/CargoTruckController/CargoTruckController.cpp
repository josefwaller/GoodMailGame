#include "CargoTruckController.h"
#include "Entity/Entity.h"
#include "Component/TransitStop/TransitStop.h"
#include "Component/MailContainer/MailContainer.h"
#include "Game/Game.h"
#include <stdexcept>

CargoTruckController::CargoTruckController(CargoTruckRoute route, std::weak_ptr<Entity> d): depot(d) {
	// Add all the locations to stops
	std::vector<sf::Vector2i> stops;
	for (CargoTruckStop stop : route.stops) {
		if (auto s = stop.target.lock()) {
#ifdef _DEBUG
			if (!s->transitStop) {
				throw std::runtime_error("Cargo truck has stop that does not have TransitStop!");
			}
#endif
			stops.push_back(sf::Vector2i(s->transitStop->getTransitLocation()));
		}
	}
	stops.push_back(sf::Vector2i(depot.lock()->transitStop->getTransitLocation()));
	this->setStops(stops);
}
void CargoTruckController::onArriveAtDest() {
	// Give letters to depot and destory self
	this->getEntity()->mailContainer->transferAllMailTo(this->depot.lock()->mailContainer);
	this->getEntity()->getGame()->removeEntity(this->getEntity());
}
float CargoTruckController::getSpeed() { return 1.0f; };
