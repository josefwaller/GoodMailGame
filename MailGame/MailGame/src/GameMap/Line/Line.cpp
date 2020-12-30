#include "Line.h"
#include <stdlib.h>

Line::Line(sf::Vector2i start, unsigned len, bool isVertical) : start(start), len(len), isVertical(isVertical) {
}

bool Line::isNextTo(Line other) {
	// First check they are both vertical/horizontal
	if (this->isVertical != other.isVertical) {
		return false;
	}
	if (this->isVertical) {
		// If they are one away on the x axis and the lines overlap in the y axis, they are directly beside each other
		return Line::isDirectlyBeside(this->start.x, other.start.x) && Line::rangesOverlap(this->start.y, this->len, other.start.y, other.len);
	}
	else {
		// Similarly, if one away on the y axis and overlapping on the x axis, they are beside each other
		return Line::isDirectlyBeside(this->start.y, other.start.y) && Line::rangesOverlap(this->start.x, this->len, other.start.x, other.len);
	}
}

bool Line::numInRange(unsigned num, unsigned start, unsigned end) {
	return num >= start && num <= start + end;
}

bool Line::rangesOverlap(unsigned startOne, unsigned endOne, unsigned startTwo, unsigned endTwo) {
	// Check if either startOne or endOne is inbetween startTwo and endTwo
	if (numInRange(startOne, startTwo, endTwo) || numInRange(endOne, startTwo, endTwo)) {
		return true;
	}
	// Check if startTwo or endTwo is inbetween startOne and endOne
	return numInRange(startTwo, startOne, endOne) || numInRange(endTwo, startOne, endOne);
}
bool Line::isDirectlyBeside(unsigned one, unsigned two) {
	return abs((int)(one - two)) <= 1;
}