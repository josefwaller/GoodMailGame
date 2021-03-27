#include "EventManager.h"
#include "Game/Game.h"
#include "TechTree/TechTree.h"

EventManager::EventManager(Game* g) : game(g) {}

void EventManager::onMailDelivered(Mail m) {
	EventManager::lettersDelivered++;
	if (EventManager::lettersDelivered == 20) {
		TechTree::unlockTech(Technology::CargoTrucks);
	}
}