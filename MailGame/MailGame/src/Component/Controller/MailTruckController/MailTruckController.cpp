#include "MailTruckController.h"
#include <stdexcept>
#include "Entity/Entity.h"
#include "Component/Transform/Transform.h"
#include <cmath>

const float MailTruckController::SPEED = 1.0f;

MailTruckController::MailTruckController(MailTruckRoute r): route(r), pointIndex(0), stopIndex(0) {
	pathfindToNextStop();
}

void MailTruckController::update(float delta) {
#ifdef _DEBUG
	if (this->pointIndex >= this->points.size()) {
		throw std::runtime_error("pointIndex > points.size() :(");
	}
#endif
	sf::Vector2f point(this->points[this->pointIndex]);
	auto trans = this->getEntity()->transform;
	sf::Vector2f pos = trans->getPosition();
	if (pow(pos.x - point.x, 2) + pow(pos.y - point.y, 2) <= pow(0.1f, 2)) {
		// Go to the next stop
		stopIndex = (stopIndex + 1) % this->route.stops.size();
		pathfindToNextStop();
	}
	else {
		// Move closer to stop
		sf::Vector2f diff = point - pos;
		float magDiff = sqrt(pow(diff.x, 2) + pow(diff.y, 2));
		trans->setPosition(trans->getPosition() + (diff / magDiff) * SPEED * delta);
	}
}
void MailTruckController::pathfindToNextStop() {
	// Need to pathfind from current position to the stop
	//tba
	this->points = { this->route.stops[stopIndex].target.value_or(sf::Vector2i(0, 0)) };
	this->pointIndex = 0;
}
