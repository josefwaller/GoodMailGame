#include "RailsPathfinder.h"
#include "System/IsoRotation/IsoRotation.h"
#include "GameMap/Tile/Tile.h"
#include "Entity/Entity.h"
#include "Game/Game.h"
#include <algorithm>
#include "GameMap/GameMap.h"

using Segment = RailsPathfinder::Segment;

Segment::Segment(Tunnel t) {
	this->type = Segment::Type::Tunnel;
	this->tunnel = t;
}
Segment::Segment(std::vector<std::pair<sf::Vector2i, Railway>> path) {
	this->type = Segment::Type::Path;
	this->path = path;
}

Segment::Type Segment::getType() {
	return this->type;
}

std::vector<std::pair<sf::Vector2i, Railway>> Segment::getPath() {
	return this->path.value();
}
Tunnel Segment::getTunnel() {
	return this->tunnel.value();
}

std::vector<SpeedPoint> RailsPathfinder::findPathBetweenPoints(
	sf::Vector3f from,
	sf::Vector3f to,
	gtime_t departTime,
	float speed) {
	return {};
}
std::vector<std::vector<RailsPathfinder::Segment>> RailsPathfinder::findRailwayPath(
	sf::Vector2i fromTile,
	sf::Vector2i toTile,
	IsoRotation startingRotation,
	GameMap* gMap
) {
	// Utility typedef
	typedef std::pair<sf::Vector2i, IsoRotation> railwayPart;

	// Struct of a path up to a point
	struct Path {
		Path(railwayPart p, std::vector<railwayPart> prev) : current(p), previous(prev) {}
		railwayPart current;
		std::vector<railwayPart> previous;
	};
	std::vector<Path> currentPaths = { Path({ fromTile, startingRotation }, {}) };
	std::vector<std::vector<RailsPathfinder::Segment>> toReturn;

	while (!currentPaths.empty()) {
		Path p = currentPaths.back();
		currentPaths.pop_back();
		// Check if it's the desintation
		if (p.current.first == toTile) {
			std::vector<std::pair<sf::Vector2i, Railway>> path;
			IsoRotation rot = startingRotation;
			// We want to skip the first element of previous since that is the starting tile
			// And we don't include that in the returned path
			for (auto it = p.previous.begin() + 1; it != p.previous.end(); it++) {
				railwayPart pt = *it;
				path.push_back({ pt.first, Railway(rot.getReverse(), pt.second) });
				rot = pt.second.getReverse();
			}
			// For right now, we just create one big segment
			toReturn.push_back({ Segment(path) });
		}
		else {
			// Get the tile and ingoing direction if the train were to follow this path
			sf::Vector2i tilePos = p.current.first + sf::Vector2i(p.current.second.getUnitVector());
			IsoRotation ingoingRotation = p.current.second;
			// Go through all the railways for this tile
			for (IsoRotation r : gMap->getTileAt(tilePos).getOutgoingRailDirections(ingoingRotation.getReverse())) {
				std::vector<railwayPart> newPath = p.previous;
				newPath.push_back(p.current);
				railwayPart toAdd = { tilePos, r };
				// Check that we are not adding a cycle
				if (std::find_if(newPath.begin(), newPath.end(), [toAdd](railwayPart p) { return p.first == toAdd.first && p.second == toAdd.second; }) == newPath.end()) {
					currentPaths.push_back(Path(toAdd, newPath));
				}
			}
		}
	}
	return toReturn;
}
/*// The tiles it has already been to
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
	// There may only be one tile in the way
	if (startingTile == toTile) {
		auto rots = gMap->getTileAt(toTile).getOutgoingRailDirections(startingRotation.getReverse());
		if (!rots.empty()) {
			// They can go directly to the tile
			return { {} };
		}
	}
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
						return { std::vector<Segment>({ Segment(path) }) };
					}
					outgoingRot = current.second.getReverse();
					current = previous.at(current);
				}
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
}*/

std::vector<SpeedPoint> RailsPathfinder::railwayPathToSpeedPointPath(std::vector<std::pair<sf::Vector2i, Railway>> path) {
	if (path.empty()) {
		return {};
	}
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