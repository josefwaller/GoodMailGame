#include "EventManager.h"
#include "Game/Game.h"
#include "TechTree/TechTree.h"

EventManager::EventManager(Game* g) : game(g), lettersByCargoTruck(0), lettersByTrain(0), lettersByPlane(0) {}

void EventManager::onMailDelivered(Mail m) {
	this->lettersDelivered++;
	if (EventManager::lettersDelivered == 20) {
		this->game->getTechTree()->unlockTech(Technology::CargoTrucks);
	}
	if (m.hasEvent(MailEvent::OnCargoTruck)) {
		this->lettersByCargoTruck++;
		if (this->lettersByCargoTruck == 20) {
			this->game->getTechTree()->unlockTech(Technology::Trains);
		}
		else if (this->lettersByCargoTruck == 100) {
			this->game->getTechTree()->unlockTech(Technology::FastCargoTrucks);
		}
	}
	if (m.hasEvent(MailEvent::OnTrain)) {
		this->lettersByTrain++;
		if (this->lettersByTrain == 20) {
			this->game->getTechTree()->unlockTech(Technology::Airplanes);
		}
		else if (this->lettersByTrain == 100) {
			this->game->getTechTree()->unlockTech(Technology::FastTrains);
		}
	}
	if (m.hasEvent(MailEvent::OnAirplane)) {
		this->lettersByPlane++;
		if (this->lettersByPlane == 100) {
			this->game->getTechTree()->unlockTech(Technology::FastAirplanes);
		}
	}
}