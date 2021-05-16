#include "IsoRotation.h"

IsoRotation::IsoRotation() : rotation(NORTH) {}
IsoRotation::IsoRotation(unsigned int rotation) : rotation(rotation% MAX_ROT) {}
unsigned int IsoRotation::getRotation() { return this->rotation; }
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
bool IsoRotation::operator==(IsoRotation other) {
	return this->getRotation() == other.getRotation();
}

bool IsoRotation::operator!=(IsoRotation other) {
	return !(*this == other);
}
IsoRotation IsoRotation::getReverse() {
	return IsoRotation(this->rotation + 4);
}
// Get unit vector
sf::Vector2f IsoRotation::getUnitVector() {
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
// Get an IsoRotation from a unit vector, assuming it's a valid one
IsoRotation IsoRotation::fromUnitVector(sf::Vector2f u) {
	if (u.x == 0.0f) {
		if (u.y == -1.0f) {
			return IsoRotation::NORTH;
		}
		else if (u.y == 1.0f) {
			return IsoRotation::SOUTH;
		}
	}
	else if (u.y == 0.0f) {
		if (u.x == 1.0f) {
			return IsoRotation::EAST;
		}
		else if (u.x == -1.0f) {
			return IsoRotation::WEST;
		}
	}
	// ERROR but not crashing as planes will prob get here
	return IsoRotation::NORTH;
}
IsoRotation IsoRotation::fromUnitVector(sf::Vector3f u) {
	return fromUnitVector(sf::Vector2f(u.x, u.y));
}
// Get unit vector in 3D space (with Z = 0)
sf::Vector3f IsoRotation::getUnitVector3D() {
	return sf::Vector3f(getUnitVector().x, getUnitVector().y, 0);
}

const std::vector<IsoRotation> IsoRotation::CARDINAL_DIRECTIONS = {
		IsoRotation(NORTH),
		IsoRotation(EAST),
		IsoRotation(SOUTH),
		IsoRotation(WEST)
};