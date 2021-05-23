#pragma once
#include "Component/Pathfinder/Pathfinder.h"
#include "GameMap/Tile/Railway/Railway.h"

class GameMap;

class RailsPathfinder : public Pathfinder {
public:
	/*
	 * Outdated method that just returns an empty path
	 */
	virtual std::vector<SpeedPoint> findPathBetweenPoints(
		sf::Vector3f from,
		sf::Vector3f to,
		gtime_t departTime,
		float speed) override;

	/*
	 * Find a path between fromTile and toTile using railways
	 * Assumes the train is leaving fromTile in the drection specified by startingRotation
	 * i.e. startingTile is (0, 1) and startingRotation is north, it assumes that the train is inbetween (0, 1) and (0, 2) heading north
	 */
	static std::vector<std::pair<sf::Vector2i, Railway>> findRailwayPath(
		sf::Vector2i fromTile,
		sf::Vector2i toTile,
		IsoRotation startingRotation,
		GameMap* gMap,
		gtime_t departTime
	);
	/**
	 * Turn a vector of tiles and railways into a vector of 3D points for the vehicle to go over
	 */
	static std::vector<SpeedPoint> railwayPathToSpeedPointPath(
		std::vector<std::pair<sf::Vector2i, Railway>> path
	);
};
