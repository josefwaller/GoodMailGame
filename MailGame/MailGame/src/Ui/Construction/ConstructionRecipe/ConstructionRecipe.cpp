#include "ConstructionRecipe.h"
#include "System/Utils/Utils.h"
#include "Game/Game.h"

ConstructionRecipe::ConstructionRecipe(
	std::string d,
	std::function<std::shared_ptr<Entity>(Game * g, sf::Vector3f pos, IsoRotation rot)> func,
	IsoSprite sprs,
	ConstructionLayout l,
	Technology t
) : layout(l), displaySprites(sprs), requiredTech(t), displayName(d) {
	this->buildFunction = func;
}

bool ConstructionRecipe::isValid(Game* g, sf::Vector3f pos3D, IsoRotation rot) {
	sf::Vector3i posToCheck3D = roundCoords(pos3D);
	sf::Vector2i posToCheck(posToCheck3D.x, posToCheck3D.y);
	// Rotate
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

void ConstructionRecipe::renderConstructionSprite(Game* g, sf::Vector3f pos, IsoRotation rot, sf::RenderWindow* w) {
	// Get the index of the sprite it should draw
	IsoRotation r = rot + g->getRotation();
	sf::Sprite displaySprite = this->displaySprites.getSprite(r);
	// Set up for building
	displaySprite = Utils::setupBuildingSprite(displaySprite);
	// Set pos
	displaySprite.setPosition(g->worldToScreenPos(sf::Vector3f(roundCoords(pos)) + sf::Vector3f(0.5f, 0.5f, 0)));
	// Set color
	if (isValid(g, pos, rot)) {
		displaySprite.setColor(sf::Color::Green);
	}
	else {
		displaySprite.setColor(sf::Color::Red);
	}
	w->draw(displaySprite);
}

std::shared_ptr<Entity> ConstructionRecipe::buildRecipe(Game* g, sf::Vector3f pos, IsoRotation rot) {
	sf::Vector3i tilePos = roundCoords(pos);
	// Create building
	std::shared_ptr<Entity> e = this->buildFunction(g, sf::Vector3f(tilePos), rot);
	// Set as the building for the tiles it's on
	g->getGameMap()->setBuildingForTile(tilePos.x, tilePos.y, e);
	// Return the entity
	return e;
}

sf::Vector3i ConstructionRecipe::roundCoords(sf::Vector3f pos) {
	return { (int)floor(pos.x), (int)floor(pos.y), (int)floor(pos.z) };
}

Technology ConstructionRecipe::getRequiredTech() {
	return this->requiredTech;
}
std::string ConstructionRecipe::getDisplayName() {
	return this->displayName;
}
