#pragma once
#include "Component/TransitStop/TransitStop.h"

/*
 * A transit stop which is the tile directly in front of the entity
 * For buildings, this is the road
 */
class BuildingTransitStop : public TransitStop {
public:
	virtual sf::Vector2f getTransitLocation() override;
};
