#pragma once
#include "Component/Pathfinder/Pathfinder.h"
#include "GameMap/Tile/Railway/Railway.h"

class GameMap;

class RailsPathfinder : public Pathfinder {
public:
	class Segment {
	public:
		enum class Type {
			Path,
			Tunnel,
			Station
		};
		Segment(Tunnel t, bool isReversed = false);
		Segment(std::vector<std::pair<sf::Vector2i, Railway>> p);
		Type getType();
		Tunnel getTunnel();
		bool getTunnelReversed();
		std::vector<std::pair<sf::Vector2i, Railway>> getPath();
	private:
		Type type;
		std::optional<Tunnel> tunnel;
		bool tunnelIsReversed;
		std::optional<std::vector<std::pair<sf::Vector2i, Railway>>> path;
	};
	/*
	 * Outdated method that just returns an empty path
	 */
	virtual std::vector<SpeedPoint> findPathBetweenPoints(
		sf::Vector3f from,
		sf::Vector3f to,
		gtime_t departTime,
		float speed) override;

	/*
	 * Find all the paths starting at fromTile and ending at any tile in toTiles using railways
	 * Assumes the train is leaving fromTile in the drection specified by startingRotation
	 * i.e. startingTile is (0, 1) and startingRotation is north, it assumes that the train is inbetween (0, 1) and (0, 2) heading north
	 * includes every tile between fromTile and toTile and toTile, but not fromTile
	 */
	static std::vector<std::vector<Segment>> findRailwayPath(
		sf::Vector2i fromTile,
		std::vector<sf::Vector2i> toTile,
		IsoRotation startingRotation,
		GameMap* gMap
	);
	/**
	 * Turn a vector of tiles and railways into a vector of 3D points for the vehicle to go over
	 */
	static std::vector<SpeedPoint> railwayPathToSpeedPointPath(
		std::vector<std::pair<sf::Vector2i, Railway>> path
	);
};
