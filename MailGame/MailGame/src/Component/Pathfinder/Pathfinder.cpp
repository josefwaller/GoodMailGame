#include "Pathfinder.h"
#include "System/Utils/Utils.h"
#include "Game/Game.h"
#include <queue>

gtime_t Pathfinder::getTimeBetween(sf::Vector3f one, sf::Vector3f two, float speed) {
	return (gtime_t)((float)Game::UNITS_IN_GAME_HOUR * (Utils::getVectorDistance(one, two) / speed));
}

std::vector<sf::Vector2i> Pathfinder::findBoatPath(GameMap* gMap, sf::Vector2i start, sf::Vector2i end) {
	std::queue<sf::Vector2i> potentialTiles;
	std::vector<sf::Vector2i> visitedTiles;

	auto previous = std::map<sf::Vector2i, sf::Vector2i, std::function<bool(sf::Vector2i one, sf::Vector2i two)>>{
		[](sf::Vector2i one, sf::Vector2i two) {
			return 1000 * one.x + one.y > 1000 * two.x + two.y;
		}
	};
	potentialTiles.push(start);
	while (!potentialTiles.empty()) {
		sf::Vector2i t = potentialTiles.front();
		potentialTiles.pop();
		// Check we haven't been here before
		if (std::find(visitedTiles.begin(), visitedTiles.end(), t) != visitedTiles.end()) {
			continue;
		}
		visitedTiles.push_back(t);
		// Add ajacent tiles
		for (int i = -1; i < 2; i++) {
			for (int j = -1; j < 2; j++) {
				if ((i == 0 || j == 0) && i != j) {
					sf::Vector2i toAdd = t + sf::Vector2i(i, j);
					if (gMap->getTileAt(toAdd).type != TileType::OffMap) {
						// Check if it's end
						if (t == end) {
							std::vector<sf::Vector2i> toReturn = { t };
							while (t != start) {
								t = previous.at(t);
								toReturn.push_back(t);
							}
							std::reverse(toReturn.begin(), toReturn.end());
							return toReturn;
						}
						// Make sure it's underwater
						size_t n = 0;
						for (size_t x = 0; x < 2; x++) {
							for (size_t y = 0; y < 2; y++) {
								if (gMap->getPointHeight(toAdd.x + x, toAdd.y + y) <= GameMap::SEA_LEVEL) {
									n++;
								}
							}
						}
						if (n < 2) {
							continue;
						}

						potentialTiles.push(toAdd);
						previous.insert({ toAdd, t });
					}
				}
			}
		}
	}
	return {};
}

Pathfinder::RoadSegment::RoadSegment(std::variant<sf::Vector2i, Tunnel> d, bool r): data(d), tunnelReversed(r) {}
sf::Vector2i Pathfinder::RoadSegment::getTile() {
	return std::get<sf::Vector2i>(this->data);
}
Tunnel Pathfinder::RoadSegment::getTunnel() {
	return std::get<Tunnel>(this->data);
}
Pathfinder::RoadSegment::Type Pathfinder::RoadSegment::getType() {
	return std::holds_alternative<sf::Vector2i>(this->data) ? Type::Tile : Type::Tunnel;
}
bool Pathfinder::RoadSegment::getTunnelReversed() {
	return this->tunnelReversed;
}
std::vector<Pathfinder::RoadSegment> Pathfinder::findCarPath(GameMap* gMap, sf::Vector2i start, sf::Vector2i end) {
	using Point = std::variant<sf::Vector2i, Tunnel>;
	auto tunnelFind = [](Point p, Tunnel t) {
		return std::holds_alternative<Tunnel>(p) && std::get<Tunnel>(p) == t;
	};
	auto tileFind = [](Point p, sf::Vector2i t) {
		return std::holds_alternative<sf::Vector2i>(p) && std::get<sf::Vector2i>(p) == t;
	};
	// Need to pathfind from current position to the destination
	// Find a path along only road from pos to stop
	std::queue<Point> potentialPoints;
	std::vector<Point> visitedPoints;
	// Map of previous position by index of visited points
	auto previous = std::map<size_t, size_t>();
	potentialPoints.push(start);
	visitedPoints.push_back(start);
	while (!potentialPoints.empty()) {
		// Get the first point
		Point p = potentialPoints.front();
		potentialPoints.pop();
		if (std::holds_alternative<sf::Vector2i>(p)) {
			sf::Vector2i point = std::get<sf::Vector2i>(p);
			size_t index = std::find_if(visitedPoints.begin(), visitedPoints.end(), [point, tileFind](Point p) { return tileFind(p, point); }) - visitedPoints.begin();
			for (Tunnel t : gMap->getTunnelsForTile(point)) {
				if (std::find_if(visitedPoints.begin(), visitedPoints.end(), [t, tunnelFind](Point p) { return tunnelFind(p, t); }) == visitedPoints.end()) {
					potentialPoints.push(t);
					visitedPoints.push_back(t);
					previous[visitedPoints.size() - 1] = index;
				}
			}
			// Ensure it has a road
			Tile fromTile = gMap->getTileAt(point.x, point.y);

			if (!fromTile.road.has_value()) {
				continue;
			}
			// Check if it is the correct one
			if (point == end) {
				// Gather points along the path
				std::vector<RoadSegment> pathPoints;
				sf::Vector2i prevPoint = end;
				while (!(std::holds_alternative<sf::Vector2i>(p) && std::get<sf::Vector2i>(p) == start)) {
					bool isReversed = false;
					if (std::holds_alternative<Tunnel>(p)) {
						// Check if we need to reverse the tunnel
						sf::Vector2i prevPoint;
						auto prev = pathPoints.back();
						if (prev.getType() == RoadSegment::Type::Tile) {
							prevPoint = prev.getTile();
						}
						else {
							prevPoint = Utils::toVector2i(prev.getTunnelReversed() ? prev.getTunnel().getEntrances().second.getPosition() : prev.getTunnel().getEntrances().first.getPosition());
						}
						auto ents = std::get<Tunnel>(p).getEntrances();
						if (ents.first.getExitTile() == prevPoint) {
							isReversed = true;
						}
					}
					pathPoints.push_back(RoadSegment(std::variant<sf::Vector2i, Tunnel>(p), isReversed));
					index = previous[index];
					p = visitedPoints.at(index);
				}
				// Add start as well
				pathPoints.push_back(RoadSegment(std::variant<sf::Vector2i, Tunnel>(start)));
				std::reverse(pathPoints.begin(), pathPoints.end());
				return pathPoints;
			}
			// Add the points around it
			std::vector<sf::Vector2i> toAdd = fromTile.road.value().getConnectedTiles(point);
			// Add any points to potential points we've not visited
			for (auto it = toAdd.begin(); it != toAdd.end(); it++) {
				if (std::find_if(visitedPoints.begin(), visitedPoints.end(), [it, tileFind](Point p) { return tileFind(p, *it); }) == visitedPoints.end()) {
					potentialPoints.push(*it);
					visitedPoints.push_back(*it);
					previous[visitedPoints.size() - 1] = index;
				}
			}
		}
		else if (std::holds_alternative<Tunnel>(p)) {
			Tunnel t = std::get<Tunnel>(p);
			size_t index = std::find_if(visitedPoints.begin(), visitedPoints.end(), [t, tunnelFind](Point p) { return tunnelFind(p, t); }) - visitedPoints.begin();
			// Can try to add either end, since one should already be visited
			std::vector<sf::Vector3i> toCheck;
			for (TunnelEntrance e : { t.getEntrances().first, t.getEntrances().second }) {
				toCheck.push_back(e.getPosition() + sf::Vector3i(e.getDirection().getReverse().getUnitVector3D()));
			}
			for (sf::Vector3i point : toCheck) {
				sf::Vector2i a = Utils::toVector2i(point);
				if (std::find_if(visitedPoints.begin(), visitedPoints.end(), [a, tileFind](Point one) { return tileFind(one, a); }) == visitedPoints.end()) {
					potentialPoints.push(a);
					visitedPoints.push_back(a);
					previous[visitedPoints.size() - 1] = index;
				}
			}
		}
	}
	throw std::runtime_error("Cannot find a path");
}

float Pathfinder::getCarRouteLength(GameMap* gMap, sf::Vector2i start, std::vector<sf::Vector2i> stops) {
	using Segment = Pathfinder::RoadSegment;
	sf::Vector2i dockPoint = start;
	sf::Vector2i lastPoint = dockPoint;
	std::vector<Segment> path = { Segment(lastPoint) };
	for (sf::Vector2i stop: stops) {
		// Time and speed shouldn't matter here
		std::vector<Segment> p = Pathfinder::findCarPath(gMap, lastPoint, stop);
		path.insert(path.end(), p.begin(), p.end());
		// Set last point
		auto var = p.back();
		if (var.getType() == Segment::Type::Tile) {
			lastPoint = var.getTile();
		}
		else {
			lastPoint = Utils::toVector2i(var.getTunnel().getEntrances().first.getPosition());
		}
	}
	// Finally it comes pack to the office
	std::vector<Segment> p = Pathfinder::findCarPath(gMap, lastPoint, dockPoint);
	path.insert(path.end(), p.begin(), p.end());
	// Now we just sum up the distance
	sf::Vector2f prev = sf::Vector2f(path.front().getTile());
	float length = 0.0f;
	for (auto it = path.begin(); it != path.end(); it++) {
		if (it->getType() == Segment::Type::Tile) {
			length += Utils::getVectorDistance(sf::Vector2f(it->getTile()), prev);
			prev = sf::Vector2f(it->getTile());
		}
		else if (it->getType() == Segment::Type::Tunnel) {
			// Since we know that the pathfinding will go directly to the entrance and exit of the tunnel, we just need to add the tunnel length
			length += it->getTunnel().getLength();
			if (it->getTunnelReversed()) {
				prev = sf::Vector2f(Utils::toVector2i(it->getTunnel().getEntrances().first.getPosition()));
			}
			else {
				prev = sf::Vector2f(Utils::toVector2i(it->getTunnel().getEntrances().second.getPosition()));
			}
		}
	}
	return length;

}