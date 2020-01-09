#pragma once
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include "Ui/Construction/Construction.h"

class Game;

class UiHandler {
public:
	// The states the UI can be in
	enum class UiState {
		Default,
		Building,
		Selecting
	};

	UiHandler(Game* g);
	bool handleEvent(sf::Event e);
	void update();
	void render(sf::RenderWindow* w);

	// Change the state
	// Also will deal with leaving the state it is currently in
	void changeState(UiState state);
private:
	Game* game;
	// Current state of the Ui
	UiState currentState;
	// Current recipe the player is building, if anything
	Construction::Recipe recipe;
	// The rotation of the current building
	IsoRotation currentRotation;
};
