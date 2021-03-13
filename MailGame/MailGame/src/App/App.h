#pragma once
#include <SFML/Graphics/RenderWindow.hpp>
#include "Game/Game.h"
#include "MainMenu/MainMenu.h"
/*
 * The entire application, contains the game, settings, menu, everything
 */
class App
{
public:
	App();
	// Run the application
	// When this ends, the app closes
	void run();

	// Generate a new game
	// Called by main menu
	void generateNewGame();
	// Load an existing save file
	// Called by main menu
	void loadSaveFile(std::string filePath);

private:
	// The window
	sf::RenderWindow window;
	// The game
	Game game;
	// The main menu
	MainMenu menu;
	// Used to calculate delta time
	float lastTime;
	// Whether the user is in main menu or ingame
	bool inMainMenu;
};
