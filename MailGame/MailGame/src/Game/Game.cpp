#include "Game.h"
#include <SFML/Graphics.hpp>

Game::Game(App* a) {

}

void Game::update(float delta) {

}

void Game::render(sf::RenderWindow* w) {
	sf::CircleShape c;
	c.setRadius(200.0f);
	c.setPosition(0.0f, 0.0f);
	c.setFillColor(sf::Color::Red);
	w->draw(c);
}