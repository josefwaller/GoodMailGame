#pragma once
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/View.hpp>
#include <memory>
#include "GameMap/GameMap.h"
#include "System/IsoRotation/IsoRotation.h"
#include "Ui/UiHandler/UiHandler.h"

class App;
class Entity;

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

	// Initialize game
	Game(App * a, sf::RenderWindow* window);
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
	sf::Vector2f getMousePosition();
	// Get the rotation of the camera
	IsoRotation getRotation();
	// Rotate the camera
	void rotateCamera();
	sf::Vector2f worldToScreenPos(sf::Vector2f pos);
	// Add/Remove entities
	void addEntity(std::shared_ptr<Entity> e);
	void removeEntity(std::weak_ptr<Entity> e);
	// Get the dimensions of the view
	sf::FloatRect getViewport();
	// Get the time
	size_t getTime();
	// Go to next hour
	void advanceTime();
	// Get a list of the entities in game
	std::vector<std::shared_ptr<Entity>> getEntities();
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
	// The time, in hours, between 0-23
	size_t time;
};
