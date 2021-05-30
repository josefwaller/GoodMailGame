#include "RoadPathfinder.h"
#include <queue>
#include "Entity/Entity.h"
#include "Game/Game.h"
#include "GameMap/GameMap.h"

std::vector<SpeedPoint> RoadPathfinder::findPathBetweenPoints(
	sf::Vector3f from,
	sf::Vector3f to,
	gtime_t departTime,
	float speed) {
	// Get Gamemap
	GameMap* gMap = this->getEntity()->getGame()->getGameMap();
	// This has been moved to Pathfinder::findCarPath
	return {};
}