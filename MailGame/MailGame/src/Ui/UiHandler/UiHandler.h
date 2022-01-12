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
		EditingPostalCodes,
		BuildingRoad,
		BuildingAirplaneRoad,
		Deleting,
		DeletingRailwaysAndRoads,
		Terraforming,
		BuildingTunnel,
		BuildingRailwaySignal,
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
	// A selection dropdown to choose a rotation from the 4 cardinal directions
	static IsoRotation chooseDirection(const char* label, IsoRotation default);
	// Select an hour
	static hour_t chooseHour(const char* label, hour_t default);
	// Set the default file name
	void setSavefileName(const char name[200]);
	// Get the tile the mouse is currently hovering over
	sf::Vector2i getHoveredTile();
	// Useful widgets
	gtime_t selectTimeOfDay(const char* label, gtime_t v);
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
	// Used for fps
	sf::Clock deltaClock;
	// The postal code the player is currently setting tiles to
	long long pCode;
	// The name of the save file being saved to
	char savefileName[200];
	// The game speeds, and the index of the current game speed
	static const std::vector<float> GAME_SPEEDS;
	size_t currentGameSpeed;
	// The start location of the road or railway currently being built
	std::optional<sf::Vector2f> startLocation;
	// Which windows are currently open
	bool roadWindowOpen;
	bool railsWindowOpen;
	bool airplaneRoadWindowOpen;
	// The railway currently being built
	bool isStation;
	bool isOneWay;
	// Is the airplane road being built a runway?
	bool isRunway;
	// If tarraforming, whether the player is raising or lowering a point
	bool isLowering;
	// Whether the visualization is turned on for postal codes/city limits/residence paths
	bool showingCityLimits;
	bool showingPostalCodes;
	bool showingResidencePaths;
	// The paths to draw
	std::vector<std::vector<RoutePoint>> pathsToDraw;
	// The entities to render UI for
	std::vector<std::weak_ptr<Entity>> toRenderUi;
	// The colors to draw the different city limits
	std::map<id_t, sf::Color> cityLimitColors;
	// The first tile the tunnel should start from, when building
	std::optional<sf::Vector2i> tunnelStart;
	TransitType tunnelType;
	// Get the point that the mouse is currently closest to
	sf::Vector2i getHoveredPoint();
	// Return a vertex array that is over the tile given, in screen coords
	// Used to drawing specific tiles
	sf::VertexArray getDrawableTile(
		sf::Vector2i pos,
		sf::PrimitiveType t = sf::PrimitiveType::Quads,
		sf::Color c = sf::Color::White
	);
	// Get the direction a tile is sloped in
	std::optional<IsoRotation> getTileSlopeDirection(sf::Vector2i exit);
	std::optional<sf::Vector2i> getTunnelExitForTile(sf::Vector2i entrance);
	// Get the height at the bottom of a slope
	std::optional<unsigned int> getBottomSlopeHeight(sf::Vector2i t);
	// Get the roads to build when the player drags to build roads
	std::vector<std::pair<sf::Vector2i, Road>> getRoadsToBuild(sf::Vector2f from, sf::Vector2f to);
	// Get the railways to build if the player clicks and drags from one point to another
	std::vector<std::pair<sf::Vector2i, Railway>> getRailwaysToBuild(sf::Vector2f from, sf::Vector2f to);
	// Utility function for IMGui checkboxes
	bool checkbox(const char* label, bool val);
	// Begin/End an entity's window
	void beginEntityWindow(std::weak_ptr<Entity> e);
	void endEntityWindow();
};
