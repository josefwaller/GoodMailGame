#pragma once
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/View.hpp>
#include <memory>
#include "GameMap/GameMap.h"
#include "System/IsoRotation/IsoRotation.h"
#include "Ui/UiHandler/UiHandler.h"
#include "Constants.h"
#include "EventManager/EventManager.h"
#include "TechTree/TechTree.h"

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

	// The number of gtime_t units in an in-game hour
	static const gtime_t UNITS_IN_GAME_HOUR;
	// The number of gtime_t units in a real world second;
	static const gtime_t UNITS_IN_REAL_SECOND;

	// To be removed
	// Whitelist of entity tags which are interlaid with the map and must be drawn instead of the tile they're on
	static const std::vector<EntityTag> WHITELIST_ENTITY_TAG;

	// Create an "empty" game, i.e. no entities, etc
	Game(App* a, sf::RenderWindow* window);
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
	// Get mouse position in game coordinates
	// Deprecated, gets the mouse position on the z = 0 plane
	sf::Vector2f getMousePosition();
	// Get the location on the ground where the mouse cursor would be
	sf::Vector3f getGroundMousePosition();
	// Get mouse position relative to the window coordinates
	sf::Vector2f getWindowMousePosition();
	// Get the rotation of the camera
	IsoRotation getRotation();
	// Rotate the camera
	void rotateCamera();
	// Transform world coordinates into screen coordinates
	sf::Vector2f worldToScreenPos(sf::Vector3f pos);
	// Transform screen coords into the resulting world coords at z = 0
	sf::Vector2f screenToWorldPos(sf::Vector2f pos);
	// Add/Remove entities
	void addEntity(std::shared_ptr<Entity> e);
	void removeEntity(std::weak_ptr<Entity> e);
	// Get the dimensions of the view
	sf::FloatRect getViewport();
	// Get the time
	gtime_t getTime();
	// Get the hour as an uint between 0 and 23
	hour_t getHour();
	// Get the hour at a certain time
	hour_t getHourAtTime(gtime_t time);
	// Get the time at midnight of today
	// Used by vehicles to determine where they should be with their departTime
	gtime_t getMidnightTime();
	// Go to next hour
	void advanceTime();
	// Get a list of the entities in game
	std::vector<std::shared_ptr<Entity>> getEntities();
	// Get an entity by id
	std::weak_ptr<Entity> getEntityById(size_t id);
	// Get the save data for the entire game
	SaveData getSaveData();
	// Toggle whether the game is paused or not
	void togglePause();
	// Get whether the game is paused
	bool getIsPaused();
	// Get the excess money the player can spend
	money_t getExcessMoney();
	// Get the total monthly budget the player has
	money_t getMonthlyBudget();
	// Check whether there is enough money to afford a cost
	// Accounts for if enforcing the budget is turned off
	bool canAffordCost(money_t cost);
	// Get the event system
	EventManager* getEventManager();
	// Get the tech tree
	TechTree* getTechTree();
	// Set the game speed
	float getGameSpeed();
	void setGameSpeed(float multiplier);

	// Get whether the game should enforce the budget or not
	bool getEnforceBudget();
	void setEnforceBudget(bool b);
private:
	// The window for the game
	sf::RenderWindow* window;
	// The entities
	// Please keep above gameMap
	std::vector<std::shared_ptr<Entity>> entities;
	// The entities to be removed at the end of the next frame
	std::vector<std::weak_ptr<Entity>> toRemove;
	// The entities to be added at the end of the next frame
	// Added after entities have been removed
	std::vector<std::shared_ptr<Entity>> toAdd;
	// The game map
	GameMap gameMap;
	// The Ui Handler
	UiHandler uiHandler;
	// The event manager
	EventManager eventManager;
	// The tech tree
	TechTree techTree;
	// The view of the game
	// Includes camera position
	sf::View gameView;
	// The rotation, where each increase is a 90 degree turn
	IsoRotation rotation;
	// The time, not in hours
	gtime_t time;
	// Whether the game is paused or not
	bool isPaused;
	// The monthly budget of the player
	money_t budget;
	bool enforceBudget;
	// The amount of money the player spends per month, currently calculated each tick
	money_t expenses;
	// The game time multiplier
	float timeMultiplier;
};
