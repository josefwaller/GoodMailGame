#pragma once
#include <SFML/Graphics/RenderWindow.hpp>
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

private:
	sf::RenderWindow window;
	// Used to calculate delta time
	float lastTime;
};

