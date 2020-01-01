#pragma once
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/View.hpp>
#include <memory>
#include "GameMap/GameMap.h"
#include "System/IsoRotation/IsoRotation.h"

class App;
class Entity;

class Game {
public:
	// How fast the camera goes
	static const float CAMERA_SPEED;
	// The width/height of a single tile on the map
	static const float TILE_WIDTH;
	static const float TILE_HEIGHT;
	Game(App * a);
	void update(float delta);
	void onEvent(sf::Event e);
	void render(sf::RenderWindow* window);

	IsoRotation getRotation();
	sf::Vector2f worldToScreenPos(sf::Vector2f pos);
	// Add/Remove entities
	void addEntity(std::shared_ptr<Entity> e);
	void removeEntity(std::shared_ptr<Entity> e);
private:
	GameMap gameMap;
	// The view of the game
	// Includes camera position
	sf::View gameView;
	// The rotation, where each increase is a 90 degree turn
	IsoRotation rotation;
	// The entities
	std::vector<std::shared_ptr<Entity>> entities;
};
