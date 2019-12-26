#include "Game.h"
#include "GameMap/GameMap.h"
#include <SFML/Graphics.hpp>

Game::Game(App* a): gameMap(this) {
}

void Game::update(float delta) {
	// Move the camera
	sf::Vector2f offset;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) {
		offset.y = -100;
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) {
		offset.y = 100;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) {
		offset.x = -100;
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) {
		offset.x = 100;
	}
	this->gameView.setCenter(this->gameView.getCenter() + offset * delta);
}

void Game::onEvent(sf::Event e) {
	const float SCROLL_SPEED = 0.1f;
	switch (e.type) {
	case sf::Event::MouseWheelMoved:
		int delta = e.mouseWheel.delta;
		gameView.zoom(1 - SCROLL_SPEED * delta);
	}
}

void Game::render(sf::RenderWindow* w) {
	w->setView(this->gameView);
	this->gameMap.render(w);
}