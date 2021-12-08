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

float Utils::getVectorDistance(sf::Vector2f one, sf::Vector2f two) {
	return sqrtf(powf(one.x - two.x, 2) + powf(one.y - two.y, 2));
}
float Utils::getVectorDistance(sf::Vector3f one, sf::Vector3f two) {
	return sqrtf(powf(one.x - two.x, 2) + powf(one.y - two.y, 2) + powf(one.z - two.z, 2));
}

sf::Vector3f Utils::getUnitVector(sf::Vector3f vector) {
	return vector / Utils::getVectorDistance(vector, sf::Vector3f());
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
// Gives the formula for the plane that goes through the three points provided
// in the ax + by + cz = d format
std::vector<float> Utils::getPlaneThroughPoints(sf::Vector3f A, sf::Vector3f B, sf::Vector3f C) {
	sf::Vector3f AB = B - A;
	sf::Vector3f AC = C - A;
	// Cross product
	float a = AB.y * AC.z - AC.y * AB.z;
	float b = AB.z * AC.x - AC.z * AB.x;
	float c = AB.x * AC.y - AC.x * AB.y;
	float d = (a * A.x + b * A.y + c * A.z);
	return {
		a,
		b,
		c,
		d
	};
}
sf::Vector3f Utils::getVectorPlaneIntersection(sf::Vector3f pos, sf::Vector3f dir, std::vector<float> plane) {
	// See https://math.stackexchange.com/questions/100439/determine-where-a-vector-will-intersect-a-plane
	float a = plane[0];
	float b = plane[1];
	float c = plane[2];
	float d = plane[3];
	float length = (d - a * pos.x - b * pos.y - c * pos.z) / (a * dir.x + b * dir.y + c * dir.z);
	return pos + dir * length;
}
std::vector<RoutePoint> Utils::toRoutePointVector(std::vector<sf::Vector3f> points, gtime_t time, float speed) {
	std::vector<RoutePoint> toReturn;
	sf::Vector3f lastPos = points.front();
	for (sf::Vector3f p : points) {
		time += Utils::getVectorDistance(lastPos, p) * Game::UNITS_IN_GAME_HOUR / speed;
		toReturn.push_back(RoutePoint(p, time, speed, 0.0f));
		lastPos = p;
	}
	return toReturn;
}
std::vector<RoutePoint> Utils::speedPointVectorToRoutePointVector(std::vector<SpeedPoint> points, gtime_t departTime, VehicleModel model, float startingSpeed) {
	// Always return an empty array when given no points
	if (points.empty())
		return {};

	VehicleModelInfo mInfo = VehicleModelInfo::getModelInfo(model);
	float acceleration = mInfo.getAcceleration();
	float vehicleMaxSpeed = mInfo.getSpeed();

	// We can now assume that there is at least 1 point
	// And the first point is always at departTime
	std::vector<RoutePoint> toReturn;
	// Go through the points and add them
	float speedAtPoint = startingSpeed;
	gtime_t expectedTime = departTime;
	// TODO: This should also eventually include distance
	for (auto it = points.begin(); it != points.end(); it++) {
		if (it == points.end() - 1) {
			// The last point, we don't need acceleration, which simplifies our equations significantly
			toReturn.push_back(RoutePoint(it->getPos(), expectedTime, speedAtPoint, 0.0f));
		}
		else if (it->getPos() == (it + 1)->getPos()) {
			continue;
		}
		else {
			// Get the next point
			SpeedPoint nextPoint = *(it + 1);
			float distance = Utils::getVectorDistance(it->getPos(), nextPoint.getPos());
			// The speed if it accelerated as fast as it could between the two points
			// vf^2 = vi^2 + 2ad
			float maxSpeed = sqrtf(powf(speedAtPoint, 2) + 2 * acceleration * distance);
			float speedAtNextPoint = nextPoint.getSpeed().value_or(std::min(maxSpeed, vehicleMaxSpeed));
			// It will acceleration up to the speed at the point
			// a = (vf^2 - vi^2) / (2d);
			float acc = (powf(speedAtNextPoint, 2) - powf(speedAtPoint, 2)) / (2 * distance);
			// Now we can add the route point
			toReturn.push_back(RoutePoint(it->getPos(), expectedTime, speedAtPoint, acc));
			// Finally, we compute when it should arrive at the next point
			// using the quadratic formula if there is acceleration, and just moving linearly if there is no acceleration
			float a = 0.5 * acc;
			float b = speedAtPoint;
			float c = -distance;
			float timeToGoToPoint = acc != 0 ?
				(-b + sqrtf(powf(b, 2.0f) - 4.0f * a * c)) / (2.0f * a) : (distance / speedAtPoint);
			expectedTime += (gtime_t)(timeToGoToPoint * ((float)Game::UNITS_IN_GAME_HOUR));
			speedAtPoint = speedAtNextPoint;
		}
	}
	return toReturn;
}

sf::Vector2i Utils::getUnitVector(sf::Vector2i v) {
	return toVector2i(getUnitVector(toVector3f(v)));
}

sf::Vector2i Utils::toVector2i(sf::Vector2f p) {
	return sf::Vector2i(floor(p.x), floor(p.y));
}
sf::Vector2i Utils::toVector2i(sf::Vector3f p) {
	return sf::Vector2i(floor(p.x), floor(p.y));
}
sf::Vector2i Utils::toVector2i(sf::Vector3i p) {
	return sf::Vector2i(p.x, p.y);
}
sf::Vector2f Utils::toVector2f(sf::Vector3f p) {
	return sf::Vector2f(p.x, p.y);
}
sf::Vector3f Utils::toVector3f(sf::Vector2i p) {
	return sf::Vector3f(p.x, p.y, 0.0f);
}
sf::Vector3f Utils::toVector3f(sf::Vector2f p) {
	return sf::Vector3f(p.x, p.y, 0.0f);
}
sf::Vector3i Utils::toVector3i(sf::Vector2i p) {
	return sf::Vector3i(p.x, p.y, 0);
}