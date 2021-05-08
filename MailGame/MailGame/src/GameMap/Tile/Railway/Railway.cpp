#include "Railway.h"
std::vector<IsoRotation> Railway::getOutgoingDirections(IsoRotation ingoingDirection) {
	if (ingoingDirection == from + IsoRotation::SOUTH) {
		return { to };
	}
	return {};
}