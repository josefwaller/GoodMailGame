#pragma once

// Simple struct which just loops a number over 4 values
// Used to represent the rotation of buildings/camera, etc
struct IsoRotation {
private:
	unsigned int rotation;
public:
	IsoRotation() { rotation = 0; }
	IsoRotation(unsigned int initRot) { rotation = initRot; }
	int getRotation() { return rotation; }
	// Operators
	IsoRotation operator+(unsigned int other) {
		return IsoRotation((rotation + other) % 4);
	}
	IsoRotation operator+(IsoRotation other) {
		return *this + other.getRotation();
	}
	IsoRotation operator-(IsoRotation other) {
		return *this - other.getRotation();
	}
	IsoRotation& operator++() {
		rotation = (rotation + 1) % 4;
		return *this;
	}
	IsoRotation operator++(int x) {
		rotation = (rotation + 1) % 4;
		return *this;
	}
	IsoRotation operator--() {
		rotation = (rotation - 1) % 4;
		return *this;
	}
	IsoRotation& operator--(int x) {
		rotation = (rotation - 1) % 4;
		return *this;
	}
};
