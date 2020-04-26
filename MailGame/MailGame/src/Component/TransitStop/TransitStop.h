#pragma once
#include "Component/Component.h"
#include <SFML/System/Vector3.hpp>

/*
 * Class for holding the details of a transit stop, that is, a place where a train, plane, or truck can stop
 */
class TransitStop : public Component {
public:
	// Get the location at which the vehicle should stop
	virtual sf::Vector3f getTransitLocation() = 0;
};
