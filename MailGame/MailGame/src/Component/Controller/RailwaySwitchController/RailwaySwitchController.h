#pragma once
#include "Component/Controller/Controller.h";
#include <SFML/System/Vector2.hpp>
#include <map>
#include "System/IsoRotation/IsoRotation.h"
#include "GameMap/Tile/Tile.h"

class RailwaySwitchController : public Controller {
public:
	// Struct for the change at an hour
	RailwaySwitchController(sf::Vector2i tile);

	virtual void update(float delta) override;
	virtual void renderUi() override;
private:
	sf::Vector2i tile;
};
