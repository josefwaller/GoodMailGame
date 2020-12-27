#include "Game.h"
#include "GameMap/GameMap.h"
#include "Entity/Entity.h"
#include "Entity/EntityTag/EntityTag.h"
#include "Component/Transform/Transform.h"
#include "Component/Renderer/Renderer.h"
#include "Component/Controller/Controller.h"
#include "Component/Controller/PostOfficeController/PostOfficeController.h"
#include "Component/ClickBox/ClickBox.h"
#include "System/SaveData/SaveData.h"
#include "Ui/Construction/Construction.h"
#include "TechTree/TechTree.h"
#include <SFML/Graphics.hpp>
#include <imgui.h>

const float Game::CAMERA_SPEED = 600.0f;
const float Game::TILE_WIDTH = 64.0f;
const float Game::TILE_HEIGHT = 32.0f;
const gtime_t Game::UNITS_IN_GAME_HOUR = 1 * 1000;
const gtime_t Game::UNITS_IN_REAL_SECOND = 1000;
const std::vector<EntityTag> Game::WHITELIST_ENTITY_TAG = {
	EntityTag::Residence,
	EntityTag::PostOffice,
	EntityTag::CargoTruckDepot,
	EntityTag::TrainStation,
	EntityTag::Airport
};
// Initialize Game
Game::Game(App* a, sf::RenderWindow* w): time(0), gameMap(this), uiHandler(this), window(w), rotation(IsoRotation::NORTH), entities(), budget(400) {
	this->gameView.setSize(sf::Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT));
}
void Game::generateNew() {
	// Stuff to generate a new game
	this->gameMap.generateNew();
}
void Game::loadFromSaveData(SaveData data) {
	// Load the tech tree
	TechTree::fromSaveData(data.getData("TechTree"));
	// First, create entities with the id's given but don't load anything else
	// So that entities referenced by id in the save data will be valid
	for (SaveData entData : data.getData("Entities").getDatas()) {
		// Create the entity
		auto e = entityTagToEntity(
			strToEntityTag(entData.getName()),
			this,
			sf::Vector3f(),
			IsoRotation()
		);
		// Load from data
		e->fromSaveData(entData);
		this->entities.push_back(e);
	}

	// The create the game map
	this->gameMap.loadFromSaveData(data.getData("GameMap"));

	// Then load the actual entity data
	for (SaveData entData : data.getData("Entities").getDatas()) {
		this->getEntityById(std::stoull(entData.getValue("id"))).lock()->loadComponentsFromSaveData(entData);
	}
	this->time = std::stoull(data.getValue("time"));
	this->gameView.setCenter(sf::Vector2f(
		data.getValuef("cameraX"),
		data.getValuef("cameraY")
	));
	this->isPaused = data.getValue("isPaused") == "1";
}

void Game::update(float delta) {
	// Advance time
	if (!isPaused) {
		const gtime_t lastTime = this->time / UNITS_IN_GAME_HOUR;
		this->time += (gtime_t)(delta * UNITS_IN_REAL_SECOND);
		if (lastTime < this->time / UNITS_IN_GAME_HOUR)
			this->advanceTime();
	}
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
	// Compute the cost
	expenses = 0;
	for (auto it = this->entities.begin(); it != this->entities.end(); it++) {
		if ((*it)->controller) {
			expenses += (*it)->controller->getCost();
		}
	}

	// Update UI
	this->uiHandler.update();

	// Update entities
	for (auto it = this->entities.begin(); it != this->entities.end(); it++) {
		if ((*it)->controller) {
			(*it)->controller->update(delta);
		}
	}

	// Remove entities
	for (auto it = this->toRemove.begin(); it != this->toRemove.end(); ++it) {
		this->entities.erase(
			std::remove(this->entities.begin(), this->entities.end(), it->lock())
		);
	}
	this->toRemove.clear();
}
sf::Vector2f Game::worldToScreenPos(sf::Vector3f pos) {
	sf::Vector2f pos2D(pos.x, pos.y);
	switch (this->rotation.getRotation()) {
	case IsoRotation::NORTH:
		break;
	case IsoRotation::EAST:
		pos2D = sf::Vector2f(-pos.y, pos.x);
		break;
	case IsoRotation::SOUTH:
		pos2D = sf::Vector2f(-pos.x, -pos.y);
		break;
	case IsoRotation::WEST:
		pos2D = sf::Vector2f(pos.y, -pos.x);
		break;
	}
	return sf::Vector2f(TILE_WIDTH * (pos2D.x - pos2D.y), TILE_HEIGHT * (pos2D.x + pos2D.y) - pos.z * TILE_HEIGHT * 0.75f);
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
	// Variables used in switch statement
	int delta;
	switch (e.type) {
	case sf::Event::MouseWheelMoved:
		delta = e.mouseWheel.delta;
		gameView.zoom(1 - SCROLL_SPEED * delta);
		break;
	case sf::Event::MouseButtonPressed:
		for (auto it = this->entities.begin(); it != this->entities.end(); it++) {
			if (auto cb = (*it)->clickBox) {
				/*if (cb->checkIfClicked(this->getMousePosition())) {
					auto x = 0;
				}*/
			}
		}
	}
}

void Game::render(sf::RenderWindow* w) {
	w->setView(this->gameView);
	this->gameMap.render(w);

	for (auto it = this->entities.begin(); it != this->entities.end(); it++) {
		std::shared_ptr<Entity> e = *it;
		const std::vector<EntityTag> v = WHITELIST_ENTITY_TAG;
		if (std::find(v.begin(), v.end(), e->tag) != v.end()) {
			continue;
		}
		if (e->renderer) {
			e->renderer->render(w);
		}
	}
#ifdef _DEBUG
	// Render clickboxes
	for (auto it = this->entities.begin(); it != this->entities.end(); it++) {
		std::shared_ptr<Entity> e = *it;
		if (e->clickBox) {
			e->clickBox->renderClickBox(w);
		}
	}
#endif
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

UiHandler* Game::getUi() {
	return &this->uiHandler;
}
gtime_t Game::getTime() { return this->time; }
hour_t Game::getHour() { return (this->time / UNITS_IN_GAME_HOUR) % 24; }
hour_t Game::getHourAtTime(gtime_t time) {
	return (time % (Game::UNITS_IN_GAME_HOUR * 24)) / Game::UNITS_IN_GAME_HOUR;
}
gtime_t Game::getMidnightTime() {
	return this->time - (this->time % (UNITS_IN_GAME_HOUR * 24));
}
void Game::advanceTime() {
	// Update mail records
	Mail::onTimeChanged(this->time);
	// Update entities
	for (auto it = this->entities.begin(); it != this->entities.end(); it++) {
		if ((*it)->controller) {
			(*it)->controller->onHourChange(this->getHour());
		}
	}
}
std::vector<std::shared_ptr<Entity>> Game::getEntities() { return this->entities; }

void Game::togglePause() {
	this->isPaused = !this->isPaused;
}

SaveData Game::getSaveData() {
	// Create save data for game
	SaveData sd("Game");
	// Add the game map's data
	sd.addData(this->gameMap.getSaveData());
	// Add all the entities' data
	SaveData entData("Entities");
	for (auto e : this->entities) {
		entData.addData(e->getSaveData());
	}
	sd.addData(entData);
	sd.addValue("time", this->time);
	sd.addValue("cameraX", this->gameView.getCenter().x);
	sd.addValue("cameraY", this->gameView.getCenter().y);
	sd.addValue("isPaused", this->isPaused);
	// Add technology save data
	sd.addData(TechTree::getSaveData());
	return sd;
}

std::weak_ptr<Entity> Game::getEntityById(size_t id) {
	for (auto it = this->entities.begin(); it != this->entities.end(); it++) {
		if ((*it)->getId() == id) {
			return *it;
		}
	}
	return {};
}
money_t Game::getExcessMoney() {
	return this->budget - this->expenses;
}
money_t Game::getMonthlyBudget() {
	return this->budget;
}
