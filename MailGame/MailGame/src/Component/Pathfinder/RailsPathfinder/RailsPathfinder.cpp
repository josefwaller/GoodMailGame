#include "RailsPathfinder.h"
#include "System/IsoRotation/IsoRotation.h"
#include "GameMap/Tile/Tile.h"
#include "Entity/Entity.h"
#include "Game/Game.h"
#include <algorithm>
#include <variant>
#include "GameMap/GameMap.h"

using Segment = RailsPathfinder::Segment;

Segment::Segment(Tunnel t, bool isReversed) {
	this->type = Segment::Type::Tunnel;
	this->tunnelIsReversed = isReversed;
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
bool Segment::getTunnelReversed() {
	return this->tunnelIsReversed;
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

	std::vector<std::vector<Segment>> currentPaths;
	sf::Vector2i startingTile = fromTile + sf::Vector2i(startingRotation.getUnitVector());
	for (IsoRotation r : gMap->getTileAt(startingTile).getOutgoingRailDirections(startingRotation.getReverse())) {
		currentPaths.push_back({ Segment({ {startingTile, Railway(startingRotation.getReverse(), r)} }) });
	}
	std::vector<std::vector<RailsPathfinder::Segment>> toReturn;

	while (!currentPaths.empty()) {
		std::vector<Segment> p = currentPaths.back();
		currentPaths.pop_back();
		sf::Vector2i tilePos;
		IsoRotation ingoingRotation;
		switch (p.back().getType()) {
		case Segment::Type::Path: {
			auto pair = p.back().getPath().back();
			// Get the tile and ingoing direction if the train were to follow this path
			tilePos = pair.first + sf::Vector2i(pair.second.getTo().getUnitVector());
			ingoingRotation = pair.second.getTo();
			break;
		}
		case Segment::Type::Tunnel: {
			auto entrances = p.back().getTunnel().getEntrances();
			TunnelEntrance e = TunnelEntrance(sf::Vector3i(), IsoRotation(IsoRotation::NORTH));
			if (p.back().getTunnelReversed()) {
				e = entrances.first;
			}
			else {
				e = entrances.second;
			}
			tilePos = Utils::toVector2i(e.getPosition()) + Utils::toVector2i(e.getDirection().getReverse().getUnitVector());
			ingoingRotation = e.getDirection().getReverse();
			break;
		}
		}
		// Check if it's the desintation
		if (std::find(toTiles.begin(), toTiles.end(), tilePos) != toTiles.end()) {
			// We have to quickly check that we can enter the tile from this direction
			if (!gMap->getTileAt(tilePos).getOutgoingRailDirections(ingoingRotation.getReverse()).empty()) {
				toReturn.push_back(p);
			}
		}
		else {
			// Go through all the railways for this tile
			for (IsoRotation r : gMap->getTileAt(tilePos).getOutgoingRailDirections(ingoingRotation.getReverse())) {
				std::pair<sf::Vector2i, Railway> toAdd = { tilePos, Railway(ingoingRotation.getReverse(), r) };
				// Check that we are not adding a cycle
				bool isLoop = false;
				for (Segment s : p) {
					if (s.getType() == Segment::Type::Path) {
						auto path = s.getPath();
						if (std::find_if(path.begin(), path.end(), [&toAdd](std::pair<sf::Vector2i, Railway> kv) { return kv.first == toAdd.first && kv.second == toAdd.second; }) != path.end()) {
							isLoop = true;
							break;
						}
					}
				}
				if (!isLoop) {
					std::vector<Segment> newPath = p;
					if (newPath.back().getType() != Segment::Type::Path) {
						newPath.push_back(Segment({}));
					}
					std::vector<std::pair<sf::Vector2i, Railway>> newSeg = newPath.back().getPath();
					newSeg.push_back(toAdd);
					newPath[newPath.size() - 1] = Segment(newSeg);
					currentPaths.push_back(newPath);
				}
			}
			// Go through all the tunnels
			for (Tunnel t : gMap->getTunnels(tilePos)) {
				if (std::find_if(p.begin(), p.end(), [&t](Segment s) { return s.getType() == Segment::Type::Tunnel && s.getTunnel() == t; }) == p.end()) {
					sf::Vector3i exitPos;
					IsoRotation exitDir;
					railwayPart exit;
					bool isReversed = false;
					if (t.getEntrances().first.getDirection() == ingoingRotation) {
						exitPos = t.getEntrances().second.getPosition();
						exitDir = t.getEntrances().second.getDirection();
					}
					else if (t.getEntrances().second.getDirection() == ingoingRotation) {
						exitPos = t.getEntrances().first.getPosition();
						exitDir = t.getEntrances().first.getDirection();
						isReversed = true;
					}
					else {
						continue;
					}
					std::vector<Segment> newPath = p;
					newPath.push_back(Segment(t, isReversed));
					currentPaths.push_back(newPath);
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