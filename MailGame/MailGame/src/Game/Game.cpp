#include "Game.h"
#include "GameMap/GameMap.h"
#include <SFML/Graphics.hpp>

Game::Game(App* a): gameMap(this) {
}

void Game::update(float delta) {

}

void Game::render(sf::RenderWindow* w) {
	this->gameMap.render(w);
}