#include "Game.h"
#include "GameMap/GameMap.h"
#include <SFML/Graphics.hpp>

const float Game::CAMERA_SPEED = 600.0f;
const float Game::TILE_WIDTH = 64.0f;
const float Game::TILE_HEIGHT = 32.0f;
Game::Game(App* a): gameMap(this) {
}

void Game::update(float delta) {
	// Move the camera
	sf::Vector2f offset;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) {
		offset.y = -CAMERA_SPEED;
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) {
		offset.y = CAMERA_SPEED;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) {
		offset.x = -CAMERA_SPEED;
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) {
		offset.x = CAMERA_SPEED;
	}
	this->gameView.setCenter(this->gameView.getCenter() + offset * delta);

	// Rotate (TBA, this should be in a button);
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::R)) {
		this->rotation = (this->rotation + 1) % 4;
		sf::Transform t;
		t.rotate(90.0f);
		this->gameView.setCenter(t.transformPoint(this->gameView.getCenter()));
	}
}
sf::Vector2f Game::worldToScreenPos(sf::Vector2f pos) {
	switch (this->rotation) {
	case 0:
		break;
	case 1:
		pos = sf::Vector2f(-pos.y, pos.x);
		break;
	case 2:
		pos = sf::Vector2f(-pos.x, -pos.y);
		break;
	case 3:
		pos = sf::Vector2f(pos.y, -pos.x);
		break;
	}
	return sf::Vector2f(TILE_WIDTH * (pos.x - pos.y), TILE_HEIGHT * (pos.x + pos.y));
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
int Game::getRotation() {
	return rotation;
}
