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
Game::Game(App* a): gameMap(this) {
	this->addEntity(EntityPresets::building(this, { 32.0f, 32.0f }, 0.0f));
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

	ImGui::Begin("Game Controls");
	if (ImGui::Button("Rotate")) {
		this->rotation++;
		sf::Vector2f center = this->gameView.getCenter();
		this->gameView.setCenter(-center.y / TILE_HEIGHT * TILE_WIDTH, center.x / TILE_WIDTH * TILE_HEIGHT);
	}
	ImGui::End();
}
sf::Vector2f Game::worldToScreenPos(sf::Vector2f pos) {
	switch (this->rotation.getRotation()) {
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

void Game::addEntity(std::shared_ptr<Entity> e) {
	this->entities.push_back(e);
}
void Game::removeEntity(std::shared_ptr<Entity> e) {
	// TBA
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

	for (auto it = this->entities.begin(); it != this->entities.end(); it++) {
		std::shared_ptr<Entity> e = *it;
		if (e->renderer) {
			e->renderer->render(w);
		}
	}
}
IsoRotation Game::getRotation() {
	return rotation;
}
