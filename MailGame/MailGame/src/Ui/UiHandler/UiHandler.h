#pragma once
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

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
};
