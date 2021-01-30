#include "Utils.h"
#include "Game/Game.h"
#include <math.h>

sf::Sprite Utils::setupBuildingSprite(sf::Sprite spr, bool isBuilding) {
	// Orient the tiles from the center of where the tile is
	// Note that this is not neccessarily the center of the sprite, as buildings are taller sprites
	// with the actual center of the tile they're on is in the bottom half
	spr.setOrigin(spr.getLocalBounds().width / 2, spr.getLocalBounds().height - 64);
	return spr;
}

float Utils::getVectorDistance(sf::Vector3f one, sf::Vector3f two) {
	return sqrtf(powf(one.x - two.x, 2) + powf(one.y - two.y, 2) + powf(one.z - two.z, 2));
}

sf::Color Utils::getTimeColor(gtime_t time) {
	float percent = (float)(time % (Game::UNITS_IN_GAME_HOUR * 24)) / (float)(Game::UNITS_IN_GAME_HOUR * 24);
	float hue = (percent * 360);
	float sat = 1;
	float v = 0.5;
	float chroma = ((1 - abs(2 * v - 1)) * sat);
	float hPrime = hue / 60;
	float X = chroma * (1 - abs(fmod(hPrime, 2) - 1));
	float r = 0.0f;
	float g = 0.0f;
	float b = 0.0f;
	if (hPrime <= 1) {
		r = chroma;
		g = X;
	}
	else if (hPrime <= 2) {
		r = X;
		g = chroma;
	}
	else if (hPrime <= 3) {
		g = chroma;
		b = X;
	}
	else if (hPrime <= 4) {
		g = X;
		b = chroma;
	}
	else if (hPrime <= 5) {
		r = X;
		b = chroma;
	}
	else {
		r = chroma;
		b = X;
	}
	float m = v - chroma / 2;
	sf::Color toReturn = sf::Color(255 * (r + m), 255 * (g + m), 255 * (b + m), 255);
	return toReturn;
}
std::vector<RoutePoint> Utils::toRoutePointVector(std::vector<sf::Vector3f> points, gtime_t time, float speed) {
	std::vector<RoutePoint> toReturn;
	sf::Vector3f lastPos = points.front();
	for (sf::Vector3f p : points) {
		time += Utils::getVectorDistance(lastPos, p) * Game::UNITS_IN_GAME_HOUR / speed;
		toReturn.push_back(RoutePoint(p, time));
		lastPos = p;
	}
	return toReturn;
}
std::vector<RoutePoint> Utils::speedPointVectorToRoutePointVector(std::vector<SpeedPoint> points, gtime_t departTime, float defaultSpeed) {
	// Always return an empty array when given no points
	if (points.empty())
		return {};

	// We can now assume that there is at least 1 point
	// And the first point is always at departTime
	std::vector<RoutePoint> toReturn;
	toReturn.push_back(RoutePoint(points.front().getPos(), departTime));
	// Add the rest of the points
	for (auto it = points.begin() + 1; it != points.end(); it++) {
		// Uses the formula `t = 2 * d / (vf - vi)` to get the time between points
		// Where d = distance, vf = final velocity, vi = initial velocity
		SpeedPoint prev = *std::prev(it);
		float vi = prev.getSpeed().value_or(defaultSpeed);
		float vf = it->getSpeed().value_or(defaultSpeed);
		float d = getVectorDistance(prev.getPos(), it->getPos());
		toReturn.push_back(RoutePoint(it->getPos(), toReturn.back().expectedTime + Game::UNITS_IN_GAME_HOUR * 2 * d / (vf + vi)));
	}
	return toReturn;
}

sf::Vector2i Utils::toVector2i(sf::Vector2f p) {
	return sf::Vector2i(floor(p.x), floor(p.y));
}
sf::Vector2i Utils::toVector2i(sf::Vector3f p) {
	return sf::Vector2i(floor(p.x), floor(p.y));
}