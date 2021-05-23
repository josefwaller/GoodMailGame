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
	return {};
}
std::vector<std::pair<sf::Vector2i, Railway>> RailsPathfinder::findRailwayPath(
	sf::Vector2i fromTile,
	sf::Vector2i toTile,
	IsoRotation startingRotation,
	GameMap* gMap,
	gtime_t departTime
) {
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
	if (gMap->getTileAt(fromTile).getRailways().empty()) {
		throw std::runtime_error("No path!");
		return {};
	}
	// First get all the outgoing tracks from the first tile
	sf::Vector2i startingTile = fromTile + Utils::toVector2i(startingRotation.getUnitVector());
	potential.push_back({ startingTile, startingRotation.getReverse() });
	// Now go through all the tracks
	while (!potential.empty()) {
		auto pair = potential.back();
		potential.pop_back();
		Tile t = gMap->getTileAt(pair.first);
		// Check if we've reached out desination
		if (pair.first == toTile) {
			// We need to quickly check that there's a way to enter this tile from our current tile
			if (!t.getOutgoingRailDirections(pair.second).empty()) {
				// Success!
				IsoRotation outgoingRot = pair.second.getReverse();
				railwayPart current = previous.at(pair);
				std::vector<std::pair<sf::Vector2i, Railway>> path;
				while (true) {
					// Add the railway on current
					path.push_back({ current.first, Railway(current.second, outgoingRot) });
					if (current.first == startingTile && current.second == startingRotation.getReverse()) {
						std::reverse(path.begin(), path.end());
						return path;
					}
					outgoingRot = current.second.getReverse();
					current = previous.at(current);
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
		for (IsoRotation rot : t.getOutgoingRailDirections(pair.second)) {
			// If it's leaving this tile from the east, it will arrive at the next tile from the west
			sf::Vector2i next = pair.first + Utils::toVector2i(rot.getUnitVector());
			railwayPart toAdd = { next, rot.getReverse() };
			potential.push_back(toAdd);
			previous.insert({ toAdd, pair });
		}
		visited.push_back(pair);
	}
	return {};
}

std::vector<SpeedPoint> RailsPathfinder::railwayPathToSpeedPointPath(std::vector<std::pair<sf::Vector2i, Railway>> path) {
	// Start at the middle of fromTile
	std::vector<SpeedPoint> points;
	// Go to the edge of the first value in path
	points.push_back(SpeedPoint(Utils::toVector3f(path.front().first) + sf::Vector3f(0.5f, 0.5f, 0) + path.front().second.getFrom().getUnitVector3D() * 0.5f));
	for (auto kv : path) {
		// Add a point
		// Since we want to be at the edge of the tile, we get the center of the tile coords (tile + (0.5, 0.5)) and add halfthe unit vector to that
		points.push_back(SpeedPoint(Utils::toVector3f(kv.first) + sf::Vector3f(0.5f, 0.5f, 0) + kv.second.getTo().getUnitVector3D() * 0.5f));
	}
	return points;
}