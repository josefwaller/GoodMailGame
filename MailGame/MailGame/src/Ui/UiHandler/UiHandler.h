#pragma once
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include "Ui/Construction/Construction.h"

class Game;

class UiHandler {
public:
	UiHandler(Game* g);
	bool handleEvent(sf::Event e);
	void update();
	void render(sf::RenderWindow* w);
	void onClick();
private:
	Game* game;
	// Whether the player is building something
	bool isBuilding;
	// Current recipe the player is building, if anything
	Construction::Recipe recipe;
	// The rotation of the current building
	IsoRotation currentRotation;
};
