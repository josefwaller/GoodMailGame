#pragma once
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/PrimitiveType.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include "Ui/Construction/Construction.h"
#include "Ui/Construction//ConstructionRecipe/ConstructionRecipe.h"
#include <optional>

class Game;

class UiHandler {
public:
	// The states the UI can be in
	enum class UiState {
		Default,
		BuildingEntity,
		BuildingRailTracks,
		SelectingEntity,
		SelectingTile,
		EditingPostalCodes
	};

	UiHandler(Game* g);
	bool handleEvent(sf::Event e);
	void update();
	void render(sf::RenderWindow* w);
	// Select an entity and call the callback method
	void selectEntity(std::function<void(std::weak_ptr<Entity>)> callback);
	// Select a tile and call the callback method
	void selectTile(std::function<void(sf::Vector2i)> callback);
	// Change the state
	// Also will deal with leaving the state it is currently in
	void changeState(UiState state);
	// Draw an arrow at the tile given in the direction given
	void drawArrow(sf::RenderWindow* window, sf::Vector2i tile, IsoRotation rot, bool isOutgoing, std::optional<sf::Color> color = {});
	// Ad a path to draw next render cycle
	void addPathToDraw(std::vector<RoutePoint> arrows);
private:
	Game* game;
	// Current state of the Ui
	UiState currentState;
	// Current recipe the player is building, if anything
	std::optional<ConstructionRecipe> recipe;
	// The rotation of the current building
	IsoRotation currentRotation;
	// Callback when selecting entity
	std::function<void(std::weak_ptr<Entity> e)> selectEntityCallback;
	// Callback when selecting tile
	std::function<void(sf::Vector2i)> selectTileCallback;
	// The postal code the player is currently setting tiles to
	long long pCode;
	// Get the tile the mouse is currently hovering over
	sf::Vector2i getHoveredTile();
	// Return a vertex array that is over the tile given, in screen coords
	// Used to drawing specific tiles
	sf::VertexArray getDrawableTile(
		sf::Vector2i pos,
		sf::PrimitiveType t = sf::PrimitiveType::Quads,
		sf::Color c = sf::Color::White
	);
	// A selection dropdown to choose a rotation from the 4 cardinal directions
	IsoRotation chooseDirection(const char* label, IsoRotation default);
	// The railway currently being built
	Railway toBuild;
	// The time to set the railway being built to, used to build railway switches
	hour_t toBuildTime;
	// The paths to draw
	std::vector<std::vector<RoutePoint>> pathsToDraw;
};
