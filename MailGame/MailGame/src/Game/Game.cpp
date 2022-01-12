#include "Game.h"
#include "GameMap/GameMap.h"
#include "Entity/Entity.h"
#include "Entity/EntityTag/EntityTag.h"
#include "Component/Transform/Transform.h"
#include "Component/Renderer/Renderer.h"
#include "Component/MailContainer/MailContainer.h"
#include "Component/Controller/Controller.h"
#include "Component/Controller/PostOfficeController/PostOfficeController.h"
#include "Component/ClickBox/ClickBox.h"
#include "System/SaveData/SaveData.h"
#include "System/Utils/Utils.h"
#include "Ui/Construction/Construction.h"
#include "TechTree/TechTree.h"
#include <SFML/Graphics.hpp>
#include <imgui.h>

const float Game::CAMERA_SPEED = 600.0f;
const float Game::TILE_WIDTH = 64.0f;
const float Game::TILE_HEIGHT = 32.0f;
const gtime_t Game::UNITS_IN_GAME_HOUR = 1 * 1000;
const gtime_t Game::UNITS_IN_REAL_SECOND = 100;
const std::vector<EntityTag> Game::WHITELIST_ENTITY_TAG = {
	EntityTag::Residence,
	EntityTag::PostOffice,
	EntityTag::CargoTruckDepot,
	EntityTag::TrainStation,
	EntityTag::Airport,
	EntityTag::CarDock,
	EntityTag::TrainDock,
	EntityTag::AirplaneDock
};
// Initialize Game
Game::Game(App* a, sf::RenderWindow* w) : time(0), gameMap(this), uiHandler(this), window(w), rotation(IsoRotation::NORTH), entities(), budget(400), eventManager(this), timeMultiplier(1), expenses(), isPaused(true), enforceBudget(true) {
	this->gameView.setSize(sf::Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT));
}
void Game::generateNew() {
	// Stuff to generate a new game
	this->gameMap.generateNew();
	this->deltaClock.restart();
}
void Game::loadFromSaveData(SaveData data) {
	using namespace SaveKeys;
	// Load event manager
	this->eventManager = EventManager(this, data.getData(EVENT_MANAGER));
	// Load the tech tree
	this->techTree = TechTree(data.getData(TECH_TREE));
	// Load the mail
	Mail::loadMailRecordData(data.getData(ALL_MAIL));
	// First, create entities with the id's given but don't load anything else
	// So that entities referenced by id in the save data will be valid
	for (SaveData entData : data.getData(ENTITIES).getDatas()) {
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
	this->gameMap.loadFromSaveData(data.getData(GAMEMAP));

	// Then load the actual entity data
	for (SaveData entData : data.getData(ENTITIES).getDatas()) {
		this->getEntityById(entData.getSizeT(ID)).lock()->loadComponentsFromSaveData(entData);
	}
	this->time = data.getSizeT(TIME);
	this->gameView.setCenter(sf::Vector2f(
		data.getFloat(CAMERA_X),
		data.getFloat(CAMERA_Y)
	));
	this->isPaused = data.getBool(IS_PAUSED);
	this->deltaClock.restart();
}

void Game::update(float delta) {
	// Advance time
	if (!isPaused) {
		const gtime_t lastTime = this->time / UNITS_IN_GAME_HOUR;
		const gtime_t toAdd = (gtime_t)(delta * UNITS_IN_REAL_SECOND * timeMultiplier);
		if (toAdd < UNITS_IN_REAL_SECOND) {
			this->time += toAdd;
		}
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
	for (size_t i = 0; i < this->toRemove.size(); i++) {
		auto r = this->toRemove[i];
		r.lock()->onDelete();
		this->entities.erase(
			std::remove(this->entities.begin(), this->entities.end(), r.lock())
		);
	}
	this->toRemove.clear();

	// Add entities
	this->entities.insert(this->entities.end(), this->toAdd.begin(), this->toAdd.end());
	for (std::shared_ptr<Entity> e : this->toAdd) {
		if (e->controller)
			e->controller->init();
	}
	this->toAdd.clear();

	while (this->deltaClock.getElapsedTime().asMilliseconds() < 1000.0f / 200.0f) {}
	this->deltaClock.restart();
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
	return sf::Vector2f(TILE_WIDTH * (pos2D.x - pos2D.y), TILE_HEIGHT * (pos2D.x + pos2D.y) - pos.z * TILE_HEIGHT * 0.6f);
}

void Game::addEntity(std::shared_ptr<Entity> e) {
	this->toAdd.push_back(e);
}
void Game::removeEntity(std::weak_ptr<Entity> e) {
	this->toRemove.push_back(e);
}

sf::FloatRect Game::getViewport() {
	sf::Vector2f size = this->gameView.getSize();
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

	// Sort the entities based on when they should be rendered
	std::vector<std::shared_ptr<Entity>> toRender;
	for (auto it = this->entities.begin(); it != this->entities.end(); it++) {
		const std::vector<EntityTag> v = WHITELIST_ENTITY_TAG;
		if (std::find(v.begin(), v.end(), (*it)->tag) != v.end()) {
			continue;
		}
		if ((*it)->renderer) {
			toRender.push_back(*it);
		}
	}

	IsoRotation rot = this->getRotation();
	std::sort(toRender.begin(), toRender.end(), [&rot](std::shared_ptr<Entity> e1, std::shared_ptr<Entity> e2) {
		sf::Vector3f e1Pos = e1->transform ? e1->transform->getPosition() : sf::Vector3f();
		sf::Vector3f e2Pos = e2->transform ? e2->transform->getPosition() : sf::Vector3f();
		switch (rot.getRotation()) {
		case IsoRotation::NORTH:
			return (e1Pos.x + e1Pos.y) < (e2Pos.x + e2Pos.y);
		case IsoRotation::SOUTH:
			return (e1Pos.x + e1Pos.y) > (e2Pos.x + e2Pos.y);
		case IsoRotation::EAST:
			return (e1Pos.x - e1Pos.y) < (e2Pos.x - e2Pos.y);
		case IsoRotation::WEST:
		default:
			return (e1Pos.x - e1Pos.y) > (e2Pos.x - e2Pos.y);
		}
		});

	for (auto it = toRender.begin(); it != toRender.end(); it++) {
		(*it)->renderer->render(w);
	}
	for (auto it = this->entities.begin(); it != this->entities.end(); it++) {
		if ((*it)->mailContainer) {
			(*it)->mailContainer->renderUi(w);
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
	return screenToWorldPos(this->window->mapPixelToCoords(sf::Vector2i(sf::Mouse::getPosition(*(this->window))), this->gameView));
}
bool isUnderSlope(sf::Vector2f slopePointOne, sf::Vector2f slopePointTwo, sf::Vector2f point) {
	float m = (slopePointOne.y - slopePointTwo.y) / (slopePointOne.x - slopePointTwo.x);
	float b = slopePointOne.y - slopePointOne.x * m;
	return point.x * m + b < point.y;
}
sf::Vector3f Game::getGroundMousePosition() {
	// Get the tile that is being hovered
	sf::Vector2i tile = this->uiHandler.getHoveredTile();
	// The vector pointing directly at the camera, computed via trial and error
	sf::Vector3f mouseVector(0.15f, 0.15f, 0.5f);
	mouseVector = mouseVector / sqrtf(powf(mouseVector.x, 2) + powf(mouseVector.y, 2) + powf(mouseVector.z, 2));
	//TODO maybe: Rotate the mousevector if camera is rotated
	// Get mouse position
	sf::Vector2f absoluteMousePos = this->window->mapPixelToCoords(sf::Mouse::getPosition(*(this->window)), this->gameView);
	sf::Vector2f mousePos(
		screenToWorldPos(absoluteMousePos)
	);
	// We have to do some more complicated stuff in order to figure out "which" plane we want
	// Check which triangular slice of the tile the cursor is in, by checing where the tile is rendered on the screen
	sf::Vector2f topLeft = this->worldToScreenPos(sf::Vector3f(tile.x, tile.y, this->gameMap.getPointHeight(tile)));
	sf::Vector2f topRight = this->worldToScreenPos(sf::Vector3f(tile.x + 1, tile.y, this->gameMap.getPointHeight(tile + sf::Vector2i(1, 0))));
	sf::Vector2f botLeft = this->worldToScreenPos(sf::Vector3f(tile.x, tile.y + 1, this->gameMap.getPointHeight(tile + sf::Vector2i(0, 1))));
	sf::Vector2f botRight = this->worldToScreenPos(sf::Vector3f(tile.x + 1, tile.y + 1, this->gameMap.getPointHeight(tile + sf::Vector2i(1, 1))));
	bool underTopLeftBotRight = isUnderSlope(topLeft, botRight, absoluteMousePos);
	bool underTopRightBotLeft = isUnderSlope(topRight, botLeft, absoluteMousePos);
	sf::Vector2f offset;
	if (underTopLeftBotRight) {
		if (underTopRightBotLeft) {
			offset = sf::Vector2f(0.5, 0.75f);
		}
		else {
			offset = sf::Vector2f(0, 0.5f);
		}
	}
	else {
		if (underTopRightBotLeft) {
			offset = sf::Vector2f(0.75f, 0.5f);
		}
		else {
			offset = sf::Vector2f(0.5f, 0);
		}
	}
	// Get the height
	return Utils::getVectorPlaneIntersection(sf::Vector3f(mousePos.x, mousePos.y, 0.0f), mouseVector, this->gameMap.getPlaneForTile(sf::Vector2i(tile), offset));
}
sf::Vector2f Game::getWindowMousePosition() {
	return this->window->mapPixelToCoords(sf::Mouse::getPosition(*(this->window)), this->gameView);
}
sf::Vector2f Game::screenToWorldPos(sf::Vector2f pos) {
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
	this->rotation.rotateQuaterClockwise();
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
gtime_t Game::getTimeSinceMidnight() {
	// ez
	return this->time - this->getMidnightTime();
}
void Game::advanceTime() {
	// Update mail records
	Mail::onTimeChanged(this->time);
	// Update entities
	for (auto it = this->entities.begin(); it != this->entities.end(); it++) {
		if ((*it)->controller) {
			(*it)->controller->onHourChange(this->getHour());
		}
		if ((*it)->mailContainer) {
			(*it)->mailContainer->updateMail();
		}
	}
}
std::vector<std::shared_ptr<Entity>> Game::getEntities() { return this->entities; }

void Game::togglePause() {
	this->isPaused = !this->isPaused;
}

SaveData Game::getSaveData() {
	using namespace SaveKeys;
	// Create save data for game
	SaveData sd(GAME);
	// Add event manager save data
	sd.addData(this->eventManager.getSaveData());
	// Add the game map's data
	sd.addData(this->gameMap.getSaveData());
	// Save mail
	sd.addData(Mail::getMailRecordData());
	// Add all the entities' data
	SaveData entData(ENTITIES);
	for (auto e : this->entities) {
		entData.addData(e->getSaveData());
	}
	sd.addData(entData);
	sd.addSizeT(TIME, this->time);
	sd.addFloat(CAMERA_X, this->gameView.getCenter().x);
	sd.addFloat(CAMERA_Y, this->gameView.getCenter().y);
	sd.addBool(IS_PAUSED, this->isPaused);
	// Add technology save data
	sd.addData(this->techTree.getSaveData());
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
EventManager* Game::getEventManager() {
	return &this->eventManager;
}
TechTree* Game::getTechTree() {
	return &this->techTree;
}

float Game::getGameSpeed() {
	return this->timeMultiplier;
}
void Game::setGameSpeed(float multiplier) {
	this->timeMultiplier = multiplier;
}
bool Game::getIsPaused() { return this->isPaused; }
bool Game::getEnforceBudget() { return this->enforceBudget; }
void Game::setEnforceBudget(bool b) { this->enforceBudget = b; }
bool Game::canAffordCost(money_t cost) { return !this->enforceBudget || cost < this->getExcessMoney(); }