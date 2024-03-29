#include "IsoRotation.h"
#include <math.h>
#include <cmath>

IsoRotation::IsoRotation() : rotation(NORTH) {}
IsoRotation::IsoRotation(unsigned int rotation) : rotation(rotation% MAX_ROT) {}
unsigned int IsoRotation::getRotation() const { return this->rotation; }
// Rotate 90 degrees clockwise
IsoRotation IsoRotation::operator+(unsigned int other) {
	return IsoRotation(rotation + other);
}
// Rotate 90 degrees clockwise
IsoRotation IsoRotation::operator+(IsoRotation other) {
	return IsoRotation(this->getRotation() + other.getRotation());
}
// Rotate 90 degrees counter clockwise
IsoRotation IsoRotation::operator-(IsoRotation other) {
	return IsoRotation(this->getRotation() - other.getRotation());
}
// Rotate 90 degrees clockwise
void IsoRotation::rotateQuaterClockwise() {
	rotation = (rotation + 2) % 8;
}
// Rotate 90 degrees counter clockwise
void IsoRotation::rotateQuaterCounterClockwise() {
	rotation = (rotation - 2) % 8;
}
// Check equality
bool IsoRotation::operator==(const IsoRotation& other) {
	return this->getRotation() == other.getRotation();
}

bool IsoRotation::operator!=(const IsoRotation& other) {
	return !(*this == other);
}
bool IsoRotation::operator<(const IsoRotation& other) const {
	return getRotation() < other.getRotation();
}
bool IsoRotation::operator>(const IsoRotation& other) const {
	return getRotation() > other.getRotation();
}
IsoRotation IsoRotation::getReverse() const {
	return IsoRotation(this->rotation + 4);
}
// Get unit vector
sf::Vector2f IsoRotation::getUnitVector() const {
	switch (this->getRotation()) {
	case IsoRotation::NORTH:
		return { 0, -1.0f };
	case IsoRotation::SOUTH:
		return { 0, 1.0f };
	case IsoRotation::EAST:
		return { 1.0f, 0 };
	case IsoRotation::WEST:
		return { -1.0f, 0 };
	default:
		throw std::runtime_error("getUnitVector called on an invalid IsoRotation!");
	}
}
// Get an IsoRotation from a unit vector, rounding to the nearest pi / 4
IsoRotation IsoRotation::fromUnitVector(sf::Vector2f u) {
	// We have to check if x is 0 first since atan is not defined for that
	if (u.x == 0) {
		return u.y > 0 ? IsoRotation::SOUTH : IsoRotation::NORTH;
	}
	// In regular math, +y is the up direction
	// But it is down for us, so inverse the Y direction
	u.y = -u.y;
	// Theta should be rounded to the nearest eigth turn
	const double PI = 3.14159;
	double theta = atan2(u.y, u.x);
	/*	if (u.y < 0) {
			theta += PI;
		}
		if (theta < 0) {
			theta += 2 * PI;
		}*/
		// Get the angle relative to the positive Y axis
	theta = PI / 2 - theta;
	while (theta < 0) { theta += 2 * PI; }
	size_t numEights = (size_t)round(theta / (PI / 4));
	if (numEights > 8) {
		throw std::runtime_error("i am bad at math :(");
	}
	return IsoRotation::CARDINAL_AND_ORDINAL_DIRECTIONS[numEights % IsoRotation::CARDINAL_AND_ORDINAL_DIRECTIONS.size()];
}
IsoRotation IsoRotation::fromUnitVector(sf::Vector3f u) {
	return fromUnitVector(sf::Vector2f(u.x, u.y));
}
// Get unit vector in 3D space (with Z = 0)
sf::Vector3f IsoRotation::getUnitVector3D() const {
	return sf::Vector3f(getUnitVector().x, getUnitVector().y, 0);
}
std::string IsoRotation::toString() {
#define x(r) case IsoRotation::r: return #r;
	switch (this->rotation) {
		x(NORTH)
			x(NORTH_EAST)
			x(EAST)
			x(SOUTH_EAST)
			x(SOUTH)
			x(SOUTH_WEST)
			x(WEST)
			x(NORTH_WEST)
	default: return "Invalid Rotation";
	}
}

bool IsoRotation::getIsCardinal() {
	return !this->getIsOrdinal();
}

bool IsoRotation::getIsOrdinal() {
	switch (this->getRotation()) {
	case IsoRotation::NORTH:
	case IsoRotation::EAST:
	case IsoRotation::SOUTH:
	case IsoRotation::WEST:
		return false;
	default:
		return true;
	}
}

const std::vector<IsoRotation> IsoRotation::CARDINAL_DIRECTIONS = {
		IsoRotation(NORTH),
		IsoRotation(EAST),
		IsoRotation(SOUTH),
		IsoRotation(WEST)
};
const std::vector<IsoRotation> IsoRotation::CARDINAL_AND_ORDINAL_DIRECTIONS = {
		IsoRotation(NORTH),
		IsoRotation(NORTH_EAST),
		IsoRotation(EAST),
		IsoRotation(SOUTH_EAST),
		IsoRotation(SOUTH),
		IsoRotation(SOUTH_WEST),
		IsoRotation(WEST),
		IsoRotation(NORTH_WEST)
};