#include "RailsPathfinder.h"
#include "System/IsoRotation/IsoRotation.h"
#include "GameMap/Tile.h"
#include "Entity/Entity.h"
#include "Game/Game.h"
#include <algorithm>
#include "GameMap/GameMap.h"

std::vector<sf::Vector3f> RailsPathfinder::findPathBetweenPoints(sf::Vector3f from, sf::Vector3f to) {
	// The tiles the vehicle has already gone through
	// TODO: It should be able to if it is not going in the same direction (i.e. go through north and then east)
	std::vector<sf::Vector3f> visited;
	// Go through the rail system
	sf::Vector3i currentPos(from);
	IsoRotation currentRot;
	while (currentPos != sf::Vector3i(to)) {
		// Move through the track
		Tile t = this->getEntity()->getGame()->getGameMap()->getTileAt((size_t)currentPos.x, (size_t)currentPos.y);
		if (!t.railway.has_value()) {
			// Railway is a dead end
			return {};
		}
		// Go to the next railway
		currentPos += sf::Vector3i(t.railway.value().to.getUnitVector3D());
		// TODO: Check the railway we are going to has to pointing to the railway we came from
		// Check we haven't been in this tile before
		if (std::find(visited.begin(), visited.end(), sf::Vector3f(currentPos)) != visited.end()) {
			return {};
		}
		visited.push_back(sf::Vector3f(currentPos));
	}
	return visited;
}
