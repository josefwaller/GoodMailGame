#pragma once
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/View.hpp>
#include "GameMap/GameMap.h"

class App;

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

	int getRotation();
	sf::Vector2f worldToScreenPos(sf::Vector2f pos);
	sf::Vector2f screenToWorldPos(sf::Vector2f pos);
private:
	GameMap gameMap;
	// The view of the game
	// Includes camera position
	sf::View gameView;
	// The rotation, where each increase is a 90 degree turn
	int rotation;
};
