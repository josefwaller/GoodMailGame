#pragma once
#include "SFML/System/Vector2.hpp"

/*
 * A simple line with some utility functions, used to represent roads
 * during city generation
 */
class Line {
public:
	Line(sf::Vector2i pos, unsigned len, bool isVertical);
	// Check if the line is directly next to another line
	// i.e. they are parallel and touch
	// This creates weird roads we don't want
	bool isNextTo(Line other);
	// Get values
	sf::Vector2i getStart() { return start; }
	unsigned getLength() { return len; }
	bool getIsVertical() { return isVertical; }
private:
	// The start and length of the line
	sf::Vector2i start;
	unsigned len;
	// Whether the line is vertical or horizontal
	bool isVertical;
	// Check if a number is withing a range
	static bool numInRange(unsigned num, unsigned start, unsigned end);
	// Check if two ranges overlap
	static bool rangesOverlap(unsigned startOne, unsigned lenOne, unsigned startTwo, unsigned lenTwo);
	// Just check if abs(one - two) <= 1;
	// Mainly used for self-explanatory code
	static bool isDirectlyBeside(unsigned one, unsigned two);
};
