#include "Pathfinder.h"
#include "System/Utils/Utils.h"
#include "Game/Game.h"

gtime_t Pathfinder::getTimeBetween(sf::Vector3f one, sf::Vector3f two, float speed) {
	return (gtime_t)((float)Game::UNITS_IN_GAME_HOUR * (Utils::getVectorDistance(one, two) / speed));
}
