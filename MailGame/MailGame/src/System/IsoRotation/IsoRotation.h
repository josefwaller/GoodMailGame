#pragma once
#include <SFML/System/Vector2.hpp>
#include <stdexcept>
#include <SFML/System/Vector3.hpp>

// Simple struct which just loops a number over 4 values
// Used to represent the rotation of buildings/camera, etc
struct IsoRotation {
private:
	unsigned int rotation;
public:
	// Names of rotation values
	const static unsigned int NORTH = 0;
	const static unsigned int EAST = 1;
	const static unsigned int SOUTH = 2;
	const static unsigned int WEST = 3;

	IsoRotation() { rotation = 0; }
	IsoRotation(unsigned int initRot) { rotation = initRot % 4; }
	unsigned int getRotation() { return rotation; }
	// Rotate 90 degrees clockwise
	IsoRotation operator+(unsigned int other) {
		return IsoRotation((rotation + other) % 4);
	}
	// Rotate 90 degrees clockwise
	IsoRotation operator+(IsoRotation other) {
		return IsoRotation(this->getRotation() + other.getRotation());
	}
	// Rotate 90 degrees counter clockwise
	IsoRotation operator-(IsoRotation other) {
		return IsoRotation(this->getRotation() - other.getRotation());
	}
	// Rotate 90 degrees clockwise
	IsoRotation& operator++() {
		rotation = (rotation + 1) % 4;
		return *this;
	}
	// Rotate 90 degrees clockwise
	IsoRotation operator++(int x) {
		rotation = (rotation + 1) % 4;
		return *this;
	}
	// Rotate 90 degrees counter clockwise
	IsoRotation operator--() {
		rotation = (rotation - 1) % 4;
		return *this;
	}
	IsoRotation& operator--(int x) {
		rotation = (rotation - 1) % 4;
		return *this;
	}
	// Check equality
	bool operator==(IsoRotation other) {
		return this->getRotation() == other.getRotation();
	}

	bool operator!=(IsoRotation other) {
		return !(*this == other);
	}
	// Get unit vector
	sf::Vector2f getUnitVector() {
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
	static IsoRotation fromUnitVector(sf::Vector2f u) {
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
	static IsoRotation fromUnitVector(sf::Vector3f u) {
		return fromUnitVector(sf::Vector2f(u.x, u.y));
	}
	// Get unit vector in 3D space (with Z = 0)
	sf::Vector3f getUnitVector3D() {
		return sf::Vector3f(getUnitVector().x, getUnitVector().y, 0);
	}
};
