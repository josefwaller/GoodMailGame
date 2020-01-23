#include "ConstructionRecipe.h"
#include "System/Utils/Utils.h"
#include "Game/Game.h"

ConstructionRecipe::ConstructionRecipe() {}
ConstructionRecipe::ConstructionRecipe(
	std::function<std::shared_ptr<Entity>(Game* g, sf::Vector2f pos, IsoRotation rot)> func,
	std::vector<sf::Sprite> sprs 
) {
	this->buildFunction = func;
	this->displaySprites = sprs;
}

bool ConstructionRecipe::isValid(Game* g, sf::Vector2f pos, IsoRotation rot) {
	// TBA
	return true;
}

void ConstructionRecipe::renderConstructionSprite(Game* g, sf::Vector2f pos, IsoRotation rot, sf::RenderWindow* w) {
	// Get the index of the sprite it should draw
	size_t i = 0;
	while (rot.getRotation() != 0) {
		rot--;
		i++;
	}
	sf::Sprite displaySprite = this->displaySprites[i];
	// Set up for building
	displaySprite = Utils::setupBuildingSprite(displaySprite);
	// Set pos
	displaySprite.setPosition(g->worldToScreenPos(sf::Vector2f(roundCoords(pos))));
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
	return { (int)round(pos.x), (int)round(pos.y) };
}
