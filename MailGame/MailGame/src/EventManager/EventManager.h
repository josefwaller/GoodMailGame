#pragma once
#include "Mail/Mail.h"

class Game;

/**
 * Manages achievements, unlocking technologies, etc
 */
class EventManager {
public:
	EventManager(Game* game);
	void onMailDelivered(Mail delivered);
private:
	Game* game;
	long long lettersDelivered;
	long long lettersByCargoTruck;
	long long lettersByTrain;
	long long lettersByPlane;
};
