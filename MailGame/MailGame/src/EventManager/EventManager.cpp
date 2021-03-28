#include "EventManager.h"
#include "Game/Game.h"
#include "TechTree/TechTree.h"

EventManager::EventManager(Game* g) : game(g), lettersDelivered(0), lettersByCargoTruck(0), lettersByTrain(0), lettersByPlane(0) {}
EventManager::EventManager(Game* g, SaveData data) : game(g) {
	using namespace SaveKeys;
	this->lettersDelivered = data.getSizeT(LETTERS_DELIVERED);
	this->lettersByCargoTruck = data.getSizeT(LETTERS_DELIVERED_BY_CARGO_TRUCK);
	this->lettersByTrain = data.getSizeT(LETTERS_DELIVERED_BY_TRAIN);
	this->lettersByPlane = data.getSizeT(LETTERS_DELIVERED_BY_AIRPLANE);
}

SaveData EventManager::getSaveData() {
	using namespace SaveKeys;
	SaveData data(EVENT_MANAGER);
	data.addSizeT(LETTERS_DELIVERED, this->lettersDelivered);
	data.addSizeT(LETTERS_DELIVERED_BY_CARGO_TRUCK, this->lettersByCargoTruck);
	data.addSizeT(LETTERS_DELIVERED_BY_TRAIN, this->lettersByTrain);
	data.addSizeT(LETTERS_DELIVERED_BY_AIRPLANE, this->lettersByPlane);
	return data;
}

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