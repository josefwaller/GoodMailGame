#pragma once

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
	IsoRotation(unsigned int initRot) { rotation = initRot; }
	int getRotation() { return rotation; }
	// Rotate 90 degrees clockwise
	IsoRotation operator+(unsigned int other) {
		return IsoRotation((rotation + other) % 4);
	}
	// Rotate 90 degrees clockwise
	IsoRotation operator+(IsoRotation other) {
		return *this + other.getRotation();
	}
	// Rotate 90 degrees counter clockwise
	IsoRotation operator-(IsoRotation other) {
		return *this - other.getRotation();
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
};
