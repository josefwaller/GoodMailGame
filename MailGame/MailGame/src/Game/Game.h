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
	// Get mouse position in game coordinates
	sf::Vector2f getMousePosition();
	// Get the rotation of the camera
	IsoRotation getRotation();
	void rotateCamera();
	sf::Vector2f worldToScreenPos(sf::Vector2f pos);
	// Add/Remove entities
	void addEntity(std::shared_ptr<Entity> e);
	void removeEntity(std::shared_ptr<Entity> e);
private:
	// The window for the game
	sf::RenderWindow* window;
	// The entities
	// Please keep above gameMap
	std::vector<std::shared_ptr<Entity>> entities;
	// The game map
	GameMap gameMap;
	// The Ui Handler
	UiHandler uiHandler;
	// The view of the game
	// Includes camera position
	sf::View gameView;
	// The rotation, where each increase is a 90 degree turn
	IsoRotation rotation;
};
