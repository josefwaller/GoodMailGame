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
	std::vector<sf::Vector2i> toTiles,
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
		// Get the tile and ingoing direction if the train were to follow this path
		sf::Vector2i tilePos = p.current.first + sf::Vector2i(p.current.second.getUnitVector());
		IsoRotation ingoingRotation = p.current.second;
		// Check if it's the desintation
		if (std::find(toTiles.begin(), toTiles.end(), tilePos) != toTiles.end()) {
			// We have to quickly check that we can enter the tile from this direction
			if (!gMap->getTileAt(tilePos).getOutgoingRailDirections(ingoingRotation.getReverse()).empty()) {
				std::vector<std::pair<sf::Vector2i, Railway>> path;
				std::vector<Segment> fullPath;
				IsoRotation rot = startingRotation;
				// We want to skip the first element of previous since that is the starting tile
				// And we don't include that in the returned path
				for (auto it = p.previous.begin() + 1; it != p.previous.end(); it++) {
					railwayPart pt = *it;
					if (gMap->getTileAt(pt.first).hasRailwaySignal()) {
						if (!path.empty()) {
							fullPath.push_back(Segment(path));
						}
						path = {};
					}
					path.push_back({ pt.first, Railway(rot.getReverse(), pt.second) });
					rot = pt.second;
				}
				path.push_back({ p.current.first, Railway(rot.getReverse(), p.current.second) });
				path.push_back({ tilePos, gMap->getTileAt(tilePos).getRailways().front() });
				fullPath.push_back(Segment(path));
				// For right now, we just create one big segment
				toReturn.push_back(fullPath);
			}
		}
		else {
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