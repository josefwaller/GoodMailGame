#include "Entity\Entity.h"

Entity::Entity(Game* g) {
	this->game = g;
}

Game* Entity::getGame() {
	return this->game;
}