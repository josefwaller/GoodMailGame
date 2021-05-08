#include "RailsPathfinder.h"
#include "System/IsoRotation/IsoRotation.h"
#include "GameMap/Tile/Tile.h"
#include "Entity/Entity.h"
#include "Game/Game.h"
#include <algorithm>
#include "GameMap/GameMap.h"

/*
 * This file should be rewritten, probably with the other pathfinders
 */
std::vector<SpeedPoint> RailsPathfinder::findPathBetweenPoints(
	sf::Vector3f from,
	sf::Vector3f to,
	gtime_t departTime,
	float speed) {
	// The tiles the vehicle has already gone through
	// TODO: It should be able to if it is not going in the same direction (i.e. go through north and then east)
	std::vector<SpeedPoint> visited = { from };
	// Go through the rail system
	sf::Vector3i currentPos(from);
	IsoRotation currentRot;
	Tile tile = this->getEntity()->getGameMap()->getTileAt(Utils::toVector2i(currentPos));
	// For the first step, we can go to any of the four connected tiles
	for (size_t i = 0; i < 4; i++) {
		IsoRotation rot(i);
		if (tile.railway.has_value()) {
			if (!tile.railway.value().getOutgoingDirections(rot).empty()) {
				// Start from here
				currentRot = rot;
				break;
			}
		}
	}
	while (currentPos != sf::Vector3i(to)) {
		// Move through the track
		Tile t = this->getEntity()->getGame()->getGameMap()->getTileAt((size_t)currentPos.x, (size_t)currentPos.y);
		if (!t.railway.has_value()) {
			// Railway is a dead end
			return {};
		}
		// Go to the next railway
		std::vector<IsoRotation> possibleRoutes = t.railway.value().getOutgoingDirections(currentRot);
		if (possibleRoutes.empty()) {
			return {};
		}
		currentPos += sf::Vector3i(possibleRoutes.front().getUnitVector3D());
		currentRot = possibleRoutes.front();
		// TODO: Check the railway we are going to has to pointing to the railway we came from
		// Check we haven't been in this tile before
		for (SpeedPoint p : visited) {
			if (p.getPos() == sf::Vector3f(currentPos))
				return {};
		}
		departTime += getTimeBetween(sf::Vector3f(currentPos), visited.back().getPos(), speed);
		visited.push_back(
			sf::Vector3f(currentPos)
		);
	}
	return visited;
}