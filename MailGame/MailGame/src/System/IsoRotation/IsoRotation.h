#pragma once
#include <SFML/System/Vector2.hpp>
#include <stdexcept>
#include <SFML/System/Vector3.hpp>
#include <vector>

// Simple struct which just loops a number over 4 values
// Used to represent the rotation of buildings/camera, etc
class IsoRotation {
private:
	unsigned int rotation;
public:
	// Names of rotation values
	const static unsigned int NORTH = 0;
	const static unsigned int NORTH_EAST = 1;
	const static unsigned int EAST = 2;
	const static unsigned int SOUTH_EAST = 3;
	const static unsigned int SOUTH = 4;
	const static unsigned int SOUTH_WEST = 5;
	const static unsigned int WEST = 6;
	const static unsigned int NORTH_WEST = 7;
	const static unsigned int MAX_ROT = 8;

	const static std::vector<IsoRotation> CARDINAL_DIRECTIONS;
	const static std::vector<IsoRotation> CARDINAL_AND_ORDINAL_DIRECTIONS;

	IsoRotation();
	IsoRotation(unsigned int initRot);
	unsigned int getRotation() const;
	// Rotate 90 degrees clockwise
	IsoRotation operator+(unsigned int other);
	// Rotate 90 degrees clockwise
	IsoRotation operator+(IsoRotation other);
	// Rotate 90 degrees counter clockwise
	IsoRotation operator-(IsoRotation other);
	// Rotate 90 degrees clockwise
	void rotateQuaterClockwise();
	// Rotate 90 degrees counter clockwise
	void rotateQuaterCounterClockwise();
	// Check equality
	bool operator==(const IsoRotation& other);
	bool operator!=(const IsoRotation& other);
	bool operator>(const IsoRotation& other) const;
	bool operator<(const IsoRotation& other) const;
	IsoRotation getReverse() const;
	// Get unit vector
	sf::Vector2f getUnitVector() const;
	// Get unit vector in 3D space (with Z = 0)
	sf::Vector3f getUnitVector3D() const;
	// Get an IsoRotation from a unit vector, assuming it's a valid one
	static IsoRotation fromUnitVector(sf::Vector2f u);
	static IsoRotation fromUnitVector(sf::Vector3f u);
};
