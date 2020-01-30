#include "ConstructionRecipe.h"
#include "System/Utils/Utils.h"
#include "Game/Game.h"

ConstructionRecipe::ConstructionRecipe() : layout({}) {}
ConstructionRecipe::ConstructionRecipe(
	std::function<std::shared_ptr<Entity>(Game* g, sf::Vector2f pos, IsoRotation rot)> func,
	std::vector<sf::Sprite> sprs,
	ConstructionLayout l
): layout(l) {
	this->buildFunction = func;
	this->displaySprites = sprs;
}

bool ConstructionRecipe::isValid(Game* g, sf::Vector2f pos, IsoRotation rot) {
	// Rotate
	sf::Vector2i posToCheck = roundCoords(pos);
	ConstructionLayout l = this->layout;
	switch (rot.getRotation()) {
	case IsoRotation::SOUTH:
		break;
	case IsoRotation::WEST:
		l = ConstructionLayout::rotate(l, 1);
		posToCheck.x -= l.getSize().x - 1;
		break;
	case IsoRotation::NORTH:
		l = ConstructionLayout::rotate(l, 2);
		posToCheck -= l.getSize() - sf::Vector2i(1, 1);
		break;
	case IsoRotation::EAST:
		l = ConstructionLayout::rotate(l, 3);
		posToCheck.y -= l.getSize().y - 1;
		break;
	}
	// Return if it fits
	return l.fits(g->getGameMap(), posToCheck);
}

void ConstructionRecipe::renderConstructionSprite(Game* g, sf::Vector2f pos, IsoRotation rot, sf::RenderWindow* w) {
	// Get the index of the sprite it should draw
	size_t i = 0;
	IsoRotation r = rot + g->getRotation();
	while (r.getRotation() != 0) {
		r--;
		i++;
	}
	sf::Sprite displaySprite = this->displaySprites[i % 4];
	// Set up for building
	displaySprite = Utils::setupBuildingSprite(displaySprite);
	// Set pos
	displaySprite.setPosition(g->worldToScreenPos(sf::Vector2f(roundCoords(pos))));
	// Set color
	if (isValid(g, pos, rot)) {
		displaySprite.setColor(sf::Color::Green);
	}
	else {
		displaySprite.setColor(sf::Color::Red);
	}
	w->draw(displaySprite);
}

std::shared_ptr<Entity> ConstructionRecipe::buildRecipe(Game* g, sf::Vector2f pos, IsoRotation rot) {
	sf::Vector2i tilePos = roundCoords(pos);
	// Create building
	std::shared_ptr<Entity> e = this->buildFunction(g, sf::Vector2f(tilePos), rot);
	// Set as the building for the tiles it's on
	g->getGameMap()->setBuildingForTile(tilePos.x, tilePos.y, e);
	// Return the entity
	return e;
}

sf::Vector2i ConstructionRecipe::roundCoords(sf::Vector2f pos) {
	return { (int)floor(pos.x), (int)floor(pos.y) };
}
