#include "Game.h"
#include "GameMap/GameMap.h"
#include "Entity/Entity.h"
#include "Component/Transform/Transform.h"
#include "Component/Renderer/Renderer.h"
#include "Entity/EntityPresets/EntityPresets.h"
#include <SFML/Graphics.hpp>
#include <imgui.h>

const float Game::CAMERA_SPEED = 600.0f;
const float Game::TILE_WIDTH = 64.0f;
const float Game::TILE_HEIGHT = 32.0f;
Game::Game(App* a, sf::RenderWindow* w): gameMap(this), uiHandler(this), window(w), rotation(IsoRotation::NORTH) {
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

	// Update UI
	this->uiHandler.update();


	// Remove entities
	for (auto it = this->toRemove.begin(); it != this->toRemove.end(); ++it) {
		this->entities.erase(
			std::remove(this->entities.begin(), this->entities.end(), it->lock())
		);
	}
	this->toRemove.clear();
}
sf::Vector2f Game::worldToScreenPos(sf::Vector2f pos) {
	switch (this->rotation.getRotation()) {
	case IsoRotation::NORTH:
		break;
	case IsoRotation::EAST:
		pos = sf::Vector2f(-pos.y, pos.x);
		break;
	case IsoRotation::SOUTH:
		pos = sf::Vector2f(-pos.x, -pos.y);
		break;
	case IsoRotation::WEST:
		pos = sf::Vector2f(pos.y, -pos.x);
		break;
	}
	return sf::Vector2f(TILE_WIDTH * (pos.x - pos.y), TILE_HEIGHT * (pos.x + pos.y));
}

void Game::addEntity(std::shared_ptr<Entity> e) {
	this->entities.push_back(e);
}
void Game::removeEntity(std::weak_ptr<Entity> e) {
	this->toRemove.push_back(e);
}

sf::FloatRect Game::getViewport() {
	sf::Vector2f size  = this->gameView.getSize();
	sf::Vector2f pos = this->gameView.getCenter() - size / 2.0f;
	return sf::FloatRect(
		pos.x,
		pos.y,
		size.x,
		size.y
	);
}

void Game::onEvent(sf::Event e) {
	if (this->uiHandler.handleEvent(e)) {
		return;
	}
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

	for (auto it = this->entities.begin(); it != this->entities.end(); it++) {
		std::shared_ptr<Entity> e = *it;
		if (e->renderer && e->tag != EntityTag::Building && e->tag != EntityTag::PostOffice) {
			e->renderer->render(w);
		}
	}
	// Render Ui
	this->uiHandler.render(w);
}
sf::Vector2f Game::getMousePosition() {
	// Get mouse position
	sf::Vector2f pos(
		this->window->mapPixelToCoords(sf::Vector2i(sf::Mouse::getPosition(*(this->window))), this->gameView)
	);
	// Cancel out tile dimensions
	pos.x /= TILE_WIDTH;
	pos.y /= TILE_HEIGHT;
	// Convert to game coords from isometric
	sf::Vector2f gameCoords(
		(pos.x + pos.y) / 2,
		(pos.y - pos.x) / 2
	);
	// Cancel out rotation
	switch (this->rotation.getRotation()) {
	case IsoRotation::NORTH:
		break;
	case IsoRotation::EAST:
		gameCoords = sf::Vector2f(gameCoords.y, -gameCoords.x);
		break;
	case IsoRotation::SOUTH:
		gameCoords = sf::Vector2f(-gameCoords.x, -gameCoords.y);
		break;
	case IsoRotation::WEST:
		gameCoords = sf::Vector2f(-gameCoords.y, gameCoords.x);
		break;
	}
	return gameCoords;
}
void Game::rotateCamera() {
	this->rotation++;
	sf::Vector2f center = this->gameView.getCenter();
	this->gameView.setCenter(-center.y / TILE_HEIGHT * TILE_WIDTH, center.x / TILE_WIDTH * TILE_HEIGHT);
}
IsoRotation Game::getRotation() {
	return rotation;
}
GameMap* Game::getGameMap() {
	return &this->gameMap;
}
