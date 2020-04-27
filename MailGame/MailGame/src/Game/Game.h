#pragma once
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/View.hpp>
#include <memory>
#include "GameMap/GameMap.h"
#include "System/IsoRotation/IsoRotation.h"
#include "Ui/UiHandler/UiHandler.h"
#include "Constants.h"

class App;
class Entity;
class SaveData;


class Game {
public:
	// How fast the camera goes
	static const float CAMERA_SPEED;
	// The width/height of a single tile on the map
	static const float TILE_WIDTH;
	static const float TILE_HEIGHT;

	// To be removed
	// Whitelist of entity tags which are interlaid with the map and must be drawn instead of the tile they're on
	static const std::vector<EntityTag> WHITELIST_ENTITY_TAG;

	// Create an "empty" game, i.e. no entities, etc
	Game(App * a, sf::RenderWindow* window);
	// Generate a new game, with a new map and everything
	void generateNew();
	// Load a game from the save data provided
	void loadFromSaveData(SaveData data);
	// Update game
	void update(float delta);
	// On an event such as a keyboard click
	void onEvent(sf::Event e);
	// Render the game, including UI
	void render(sf::RenderWindow* window);

	// Get the game map
	GameMap* getGameMap();
	// Get the UI
	UiHandler* getUi();
	// Get mouse position in screen coordinates
	sf::Vector2f getMousePosition();
	// Get the rotation of the camera
	IsoRotation getRotation();
	// Rotate the camera
	void rotateCamera();
	// Transform world coordinates into screen coordinates
	sf::Vector2f worldToScreenPos(sf::Vector3f pos);
	// Add/Remove entities
	void addEntity(std::shared_ptr<Entity> e);
	void removeEntity(std::weak_ptr<Entity> e);
	// Get the dimensions of the view
	sf::FloatRect getViewport();
	// Get the time
	gtime_t getTime();
	// Get the hour as an uint between 0 and 23
	hour_t getHour();
	// Go to next hour
	void advanceTime();
	// Get a list of the entities in game
	std::vector<std::shared_ptr<Entity>> getEntities();
	// Get an entity by id
	std::weak_ptr<Entity> getEntityById(size_t id);
	// Get the save data for the entire game
	SaveData getSaveData();
private:
	// The window for the game
	sf::RenderWindow* window;
	// The entities
	// Please keep above gameMap
	std::vector<std::shared_ptr<Entity>> entities;
	// The entities to be removed at the end of the next frame
	std::vector<std::weak_ptr<Entity>> toRemove;
	// The game map
	GameMap gameMap;
	// The Ui Handler
	UiHandler uiHandler;
	// The view of the game
	// Includes camera position
	sf::View gameView;
	// The rotation, where each increase is a 90 degree turn
	IsoRotation rotation;
	// The time, not in hours
	gtime_t time;
};
