#pragma once
#include "Component/TransitStop/TransitStop.h"

/*
 * A basic transit stop, simply uses the location of the entity
 */
class BasicTransitStop : public TransitStop {
public:
	virtual sf::Vector2f getTransitLocation() override;
};
