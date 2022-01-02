#pragma once
#include <SFML/System/Vector3.hpp>
#include <SFML/System/Vector2.hpp>
#include <vector>
#include <variant>
#include "Component/Component.h"
#include "System/SpeedPoint/SpeedPoint.h"
#include "System/Utils/Utils.h"
#include "GameMap/Tile/Road/Road.h"
#include "GameMap/Tunnel/Tunnel.h"

class GameMap;

/*
 * A component that finds a path between two points
 * Used for vehicle transportation, can be implemented differently for different vehicles
 */
class Pathfinder : public Component {
public:
	virtual std::vector<SpeedPoint> findPathBetweenPoints(
		sf::Vector3f from,
		sf::Vector3f to,
		gtime_t departTime,
		float speed) = 0;

	// Find a path via water
	// wow real difficult
	// start and empty must both be at least adjacent to water
	static std::vector<sf::Vector2i> findBoatPath(GameMap* gMap, sf::Vector2i start, sf::Vector2i end);
	// Eventually all the pathfinding methods will be static methods here
	/**
	 * Find a path between two points along regular roads
	 * Assumes the car starts at the center of the start tile and ends at the center of the end tile
	 * Will eventually accomodate bridges/etc
	 * Returns all the tiles along the path, including start and end
	 */
	class RoadSegment {
	public:
		enum class Type {
			Tile,
			Tunnel,
		};
		RoadSegment(std::variant<sf::Vector2i, Tunnel> d, bool tunnelReversed = false);
		Type getType();
		sf::Vector2i getTile();
		Tunnel getTunnel();
		// Get whether the car should go through the tunnel backwards
		// i.e. It enters through entrances.second and exits through entrances.first
		bool getTunnelReversed();
	private:
		std::variant<sf::Vector2i, Tunnel> data;
		bool tunnelReversed;
	};
	static std::vector<RoadSegment> findCarPath(GameMap* gMap, sf::Vector2i start, sf::Vector2i end);
protected:
	gtime_t getTimeBetween(sf::Vector3f from, sf::Vector3f to, float speed);
};
