#pragma once
#include "Mail/Mail.h"
#include "System/SaveData/SaveData.h"

class Game;

/**
 * Manages achievements, unlocking technologies, etc
 */
class EventManager {
public:
	EventManager(Game* game);
	EventManager(Game* g, SaveData data);
	void onMailDelivered(Mail delivered);
	SaveData getSaveData();
private:
	Game* game;
	unsigned long long lettersDelivered;
	unsigned long long lettersByCargoTruck;
	unsigned long long lettersByTrain;
	unsigned long long lettersByPlane;
};
