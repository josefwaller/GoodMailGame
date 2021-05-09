#include "RailsPathfinder.h"
#include "System/IsoRotation/IsoRotation.h"
#include "GameMap/Tile/Tile.h"
#include "Entity/Entity.h"
#include "Game/Game.h"
#include <algorithm>
#include "GameMap/GameMap.h"

std::vector<SpeedPoint> RailsPathfinder::findPathBetweenPoints(
	sf::Vector3f from,
	sf::Vector3f to,
	gtime_t departTime,
	float speed) {
	GameMap* gMap = this->getEntity()->getGameMap();
	// Utility typedef
	typedef std::pair<sf::Vector2i, IsoRotation> railwayPart;
	// The tiles it has already been to
	std::vector<railwayPart> visited;
	std::vector<railwayPart> potential;
	std::map<railwayPart, railwayPart, bool(*)(railwayPart, railwayPart)> previous([](railwayPart one, railwayPart two) {
#define f(var) var.first.x + var.first.y * 1000 + var.second.getRotation() * 1000 * 1000
		return f(one) < f(two);
#undef f
		});
	// Check that there is a track on the from tile
	if (!gMap->getTileAt(Utils::toVector2i(from)).railway.has_value()) {
		throw std::runtime_error("No path!");
		return {};
	}
	// First get all the outgoing tracks from the first tile
	for (size_t i = 0; i < 4; i++) {
		IsoRotation r(i);
		std::vector<IsoRotation> outgoing = gMap->getTileAt(Utils::toVector2i(from)).railway.value().getOutgoingDirections(r);
		for (IsoRotation o : outgoing) {
			// Add it to potential
			potential.push_back({ Utils::toVector2i(from + o.getUnitVector3D()), o + 2 });
			previous.insert({ { Utils::toVector2i(from + o.getUnitVector3D()), o + 2 }, { Utils::toVector2i(from), o } });
		}
		visited.push_back({ Utils::toVector2i(from), r });
	}
	// Now go through all the tracks
	while (!potential.empty()) {
		auto pair = potential.back();
		potential.pop_back();
		// Check there's a railway
		if (!gMap->getTileAt(pair.first).railway.has_value()) {
			continue;
		}
		Railway r = gMap->getTileAt(pair.first).railway.value();
		// Check if we've reached out desination
		if (pair.first == Utils::toVector2i(to)) {
			// We need to quickly check that there's a way to enter this tile from our current tile
			if (gMap->getTileAt(pair.first).railway.has_value() && !gMap->getTileAt(pair.first).railway.value().getOutgoingDirections(pair.second).empty()) {
				// Success!
				railwayPart current = pair;
				std::vector<SpeedPoint> path = { SpeedPoint(Utils::toVector3f(current.first)) };
				while (true) {
					if (previous.find(current) == previous.end()) {
						std::reverse(path.begin(), path.end());
						return path;
					}
					current = previous.at(current);
					path.push_back(SpeedPoint(Utils::toVector3f(current.first)));
				}
				return {};
			}
		}
		// Make sure we've never been here before
		if (std::find_if(visited.begin(), visited.end(), [pair](auto kv) { return kv.first == pair.first && kv.second == pair.second; }) != visited.end()) {
			// TODO: Decide which path to keep
			// For right now just disregard the current path
			continue;
		}
		for (IsoRotation rot : gMap->getTileAt(pair.first).railway.value().getOutgoingDirections(pair.second)) {
			// rot + 2 since we want the opposite direction
			// If it's leaving this tile from the east, it will arrive at the next tile from the west
			sf::Vector2i next = pair.first + Utils::toVector2i(rot.getUnitVector());
			railwayPart toAdd = { next, rot + 2 };
			potential.push_back(toAdd);
			previous.insert({ toAdd, pair });
		}
	}
	return {};
}