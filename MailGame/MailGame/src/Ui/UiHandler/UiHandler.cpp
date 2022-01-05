#include "UiHandler.h"
#include "Entity/Entity.h"
#include "Mail/Mail.h"
#include "Component/ClickBox/ClickBox.h"
#include <imgui.h>
#include <iostream>
#include <fstream>
#include <imgui-SFML.h>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include "Game/Game.h"
#include "PostalCodeDatabase/PostalCodeDatabase.h"
#include "ResourceLoader/ResourceLoader.h"
#include "System/SaveData/SaveData.h"
#include "System/Utils/Utils.h"
#include "TechTree/TechTree.h"
#include "Component/Controller/Controller.h"
#include "Component/Controller/ResidenceController/ResidenceController.h"

const std::vector<float> UiHandler::GAME_SPEEDS = { 0.1f, 0.25f, 0.5f, 1.0f, 2.0f, 5.0f, 10.0f, 25.0f, 50.0f };
UiHandler::UiHandler(Game* g) : game(g), currentState(UiState::Default), recipe(), currentGameSpeed(3),
isStation(false) {
	cityLimitColors.insert({ 0, sf::Color::Blue });
}

bool UiHandler::handleEvent(sf::Event e) {
	sf::Vector2i tilePos;
	tilePos = sf::Vector2i(this->game->getMousePosition());
	switch (e.type) {
	case sf::Event::MouseButtonPressed:
		if (ImGui::GetIO().WantCaptureMouse) {
			return true;
		}
		if (e.key.code == sf::Mouse::Right) {
			this->changeState(UiState::Default);
		}
		else if (e.key.code != sf::Mouse::Left) {
			return true;
		}
		switch (this->currentState) {
		case UiState::Default: {
			// Maybe open an entity's UI
			auto e = this->game->getEntities();
			for (auto it = e.begin(); it != e.end(); it++) {
				if ((*it)->clickBox && (*it)->clickBox->checkIfClicked(this->game->getWindowMousePosition())) {
					this->toRenderUi.push_back((*it));
				}
			}
			break;
		}
		case UiState::BuildingEntity: {
			if (!this->recipe) {
				throw std::runtime_error("BuildingEntity but with no ConstructionRecipe!");
			}
			sf::Vector3f mousePos = this->game->getGroundMousePosition();
			GameMap* gMap = this->game->getGameMap();
			sf::Vector3f pos(mousePos.x, mousePos.y, std::max({
				gMap->getPointHeight(sf::Vector2i(std::floor(mousePos.x), std::floor(mousePos.y))),
				gMap->getPointHeight(sf::Vector2i(std::floor(mousePos.x), std::ceil(mousePos.y))),
				gMap->getPointHeight(sf::Vector2i(std::ceil(mousePos.x), std::floor(mousePos.y))),
				gMap->getPointHeight(sf::Vector2i(std::ceil(mousePos.x), std::ceil(mousePos.y))),
				}));
			if (this->recipe.value().isValid(this->game, pos, this->currentRotation)) {
				this->game->addEntity(
					this->recipe.value().buildRecipe(this->game, pos, this->currentRotation)
				);
			}
			this->changeState(UiState::Default);
			return true;
		}
		case UiState::BuildingRailTracks:
			this->startLocation = sf::Vector2f(this->getHoveredTile()) + sf::Vector2f(0.5f, 0.5f);
			break;
		case UiState::BuildingRoad:
		case UiState::BuildingAirplaneRoad: {
			this->startLocation = sf::Vector2f(this->getHoveredTile());
			break;
		}
		case UiState::SelectingEntity:
			// Get the entity
			for (auto e : this->game->getEntities()) {
				if (e->clickBox) {
					if (e->clickBox->checkIfClicked(this->game->getMousePosition())) {
						this->selectEntityCallback(std::weak_ptr<Entity>(e));
					}
				}
			}
			this->changeState(UiState::Default);
			break;
		case UiState::SelectingTile:
			// Call the callback
			this->selectTileCallback(this->getHoveredTile());
			this->changeState(UiState::Default);
			break;
		case UiState::Deleting: {
			sf::Vector2i tile = this->getHoveredTile();
			// Remove entity, railway, and airplane road
			const std::vector<EntityTag> DELETABLE_ENTITIES = {
				EntityTag::PostOffice,
				EntityTag::CargoTruckDepot,
				EntityTag::TrainStation,
				EntityTag::Airport,
				EntityTag::CarDock,
				EntityTag::TrainDock,
				EntityTag::AirplaneDock,
				EntityTag::Warehouse
			};
			std::weak_ptr<Entity> e = this->game->getGameMap()->getTileAt(tile.x, tile.y).building;
			if (e.lock() && std::find(DELETABLE_ENTITIES.begin(), DELETABLE_ENTITIES.end(), e.lock()->tag) != DELETABLE_ENTITIES.end()) {
				this->game->removeEntity(e);
			}
			// Remove road
			this->game->getGameMap()->removeRoad(tile.x, tile.y);
			// Remove railway
			this->game->getGameMap()->removeRailTrack(tile.x, tile.y);
			// Remove airplane road
			this->game->getGameMap()->removeAirplaneRoad(tile.x, tile.y);
			// Remove tunnels
			this->game->getGameMap()->removeTunnel(tile.x, tile.y);
			break;
		}
		case UiState::DeletingRailwaysAndRoads: {
			sf::Vector2i tile = this->getHoveredTile();
			this->game->getGameMap()->removeAirplaneRoad(tile.x, tile.y);
			this->game->getGameMap()->removeRoad(tile.x, tile.y);
			this->game->getGameMap()->removeRailTrack(tile.x, tile.y);
			break;
		}
		case UiState::Terraforming: {
			sf::Vector2i point = this->getHoveredPoint();
			int modifier = this->isLowering ? -1 : 1;
			int height = (int)this->game->getGameMap()->getPointHeight(point.x, point.y) + modifier;
			this->game->getGameMap()->setPointHeight(point.x, point.y, std::min(std::max(height, 0), 10));
			break;
		}
		case UiState::BuildingTunnel: {
			sf::Vector2i tile = this->getHoveredTile();
			if (this->getTunnelExitForTile(tile).has_value()) {
				this->game->getGameMap()->addTunnel(tile, this->getTunnelExitForTile(tile).value(), this->tunnelType);
			}
			break;
		}
		case UiState::BuildingRailwaySignal:
			this->game->getGameMap()->addRailwaySignal(this->getHoveredTile());
			break;
		}
		break;
	case sf::Event::MouseButtonReleased:
		switch (this->currentState) {
		case UiState::BuildingAirplaneRoad:
		case UiState::BuildingRoad: {
			if (this->startLocation.has_value()) {
				auto toBuild = this->getRoadsToBuild(this->startLocation.value(), sf::Vector2f(this->getHoveredTile()));
				for (std::pair<sf::Vector2i, Road> b : toBuild) {
					if (this->currentState == UiState::BuildingAirplaneRoad) {
						this->game->getGameMap()->addAirplaneRoad(b.first.x, b.first.y, AirplaneRoad(b.second, this->isRunway));
					}
					else {
						this->game->getGameMap()->addRoad(b.first.x, b.first.y, b.second);
					}
				}
			}
			this->startLocation.reset();
			break;
		}
		case UiState::BuildingRailTracks:
			if (this->startLocation.has_value()) {
				sf::Vector3f mousePos = this->game->getGroundMousePosition();
				for (auto kv : this->getRailwaysToBuild(this->startLocation.value(), sf::Vector2f(mousePos.x, mousePos.y))) {
					if (this->isStation) {
						this->game->getGameMap()->addRailwayStation(kv.first.x, kv.first.y, kv.second.getFrom());
					}
					else {
						this->game->getGameMap()->addRailTrack(kv.first.x, kv.first.y, kv.second.getFrom(), kv.second.getTo(), this->isOneWay);
					}
				}
			}
			this->startLocation.reset();
		}
		break;
	case sf::Event::KeyPressed:
		if (ImGui::GetIO().WantCaptureKeyboard) {
			return true;
		}
		if (e.key.code == sf::Keyboard::R) {
			this->currentRotation.rotateQuaterClockwise();
		}
		break;
	}
	return false;
}

// Simple utility function for calculating the area of a triangle
float triangleArea(sf::Vector2f a, sf::Vector2f b, sf::Vector2f c) {
	return std::fabsf((a.x * (b.y - c.y) + b.x * (c.y - a.y) + c.x * (a.y - b.y)) / 2);
}
sf::Vector2i UiHandler::getHoveredTile() {
	// Use this algorithm to check if the point is in a tile: https://stackoverflow.com/questions/5922027/how-to-determine-if-a-point-is-within-a-quadrilateral
	for (size_t x = 0; x < GameMap::MAP_WIDTH; x++) {
		for (size_t y = 0; y < GameMap::MAP_HEIGHT; y++) {
			// Get the points of this tile on the screen
			// Note that this does not account for the gameView transformation
			std::vector<sf::Vector2f> screenPoints = {
				this->game->worldToScreenPos(sf::Vector3f(x, y, this->game->getGameMap()->getPointHeight(x, y))),
				this->game->worldToScreenPos(sf::Vector3f(x + 1, y, this->game->getGameMap()->getPointHeight(x + 1, y))),
				this->game->worldToScreenPos(sf::Vector3f(x + 1, y + 1, this->game->getGameMap()->getPointHeight(x + 1, y + 1))),
				this->game->worldToScreenPos(sf::Vector3f(x, y + 1, this->game->getGameMap()->getPointHeight(x, y + 1))),
			};
			// Get the area of the tile
			float tileArea = triangleArea(screenPoints[0], screenPoints[1], screenPoints[2]) + triangleArea(screenPoints[2], screenPoints[3], screenPoints[0]);
			// Now calculate the area of all the triangles formed between the mouse coordinates and the tile's points
			sf::Vector2f mousePos(this->game->getWindowMousePosition());
			float pointArea = 0.0f;
			for (size_t i = 0; i < 4; i++) {
				pointArea += triangleArea(mousePos, screenPoints[i], screenPoints[(i + 1) % 4]);
			}
			// Check if they are the same (within a margin of error)
			if (abs(pointArea - tileArea) < 0.001f) {
				return sf::Vector2i(x, y);
			}
		}
	}
	sf::Vector2f mousePos = this->game->getMousePosition();
	return sf::Vector2i((int)floor(mousePos.x), (int)floor(mousePos.y));
}
sf::Vector2i UiHandler::getHoveredPoint() {
	sf::Vector2f mousePoint = this->game->getMousePosition();
	return sf::Vector2i((int)round(mousePoint.x), (int)round(mousePoint.y));
}

void UiHandler::selectTile(std::function<void(sf::Vector2i pos)> callback) {
	this->changeState(UiState::SelectingTile);
	this->selectTileCallback = callback;
}

void UiHandler::selectEntity(std::function<void(std::weak_ptr<Entity>)> callback) {
	this->changeState(UiState::SelectingEntity);
	this->selectEntityCallback = callback;
}
void UiHandler::update() {
	char buf[200];
	ImGui::Begin("Game");
	// Print available money
	sprintf_s(buf, "%d / %d CAD available", game->getExcessMoney(), game->getMonthlyBudget());
	ImGui::Text(buf);
	// Print percent of letters that have been delivered
	std::map<id_t, MailRecord> allMail = Mail::getAllMail();
	unsigned long long numDelivered = 0;
	unsigned long long numUnderDay = 0;
	for (auto it = allMail.begin(); it != allMail.end(); it++) {
		if (it->second.hasBeenDelivered) {
			numDelivered++;
		}
		if (this->game->getTime() - it->second.time <= 24 * Game::UNITS_IN_GAME_HOUR) {
			numUnderDay++;
		}
	}
	float percent = (float)numDelivered / (float)allMail.size();
	sprintf_s(buf, "%f of letters delivered", percent * 100.0f);
	ImGui::Text(buf);
	percent = (float)(numUnderDay) / (float)allMail.size();
	sprintf_s(buf, "%f of letters are less than a day old", percent * 100.0f);
	ImGui::Text(buf);

	if (ImGui::Button("Rotate Camera")) {
		this->game->rotateCamera();
	}
	ImGui::Text((std::to_string(this->game->getHour()) + ":00").c_str());
	ImGui::Text("Game Speed ");
	ImGui::SameLine();
	ImGui::Text(std::to_string(GAME_SPEEDS[this->currentGameSpeed]).c_str());
	if (ImGui::Button("<<")) {
		this->currentGameSpeed = 0;
	}
	ImGui::SameLine();
	if (ImGui::Button("<")) {
		this->currentGameSpeed = std::max(0ULL, this->currentGameSpeed - 1);
	}
	ImGui::SameLine();
	if (ImGui::Button(this->game->getIsPaused() ? ">" : "||")) {
		this->game->togglePause();
	}
	ImGui::SameLine();
	if (ImGui::Button(">")) {
		this->currentGameSpeed = std::min(GAME_SPEEDS.size() - 1, this->currentGameSpeed + 1);
	}
	ImGui::SameLine();
	if (ImGui::Button(">>")) {
		this->currentGameSpeed = GAME_SPEEDS.size() - 1;
	}
	this->game->setGameSpeed(GAME_SPEEDS[this->currentGameSpeed]);
	ImGui::Text((std::string("Game.time: ") + std::to_string(this->game->getTime())).c_str());
	ImGui::Text("Savefile: ");
	ImGui::SameLine();
	ImGui::InputText("", this->savefileName, 200);
	if (ImGui::Button("Save") && !std::string(this->savefileName).empty()) {
		// Terrible way of temporarily doing this
		// Just save rn to file
		SaveData toSave = this->game->getSaveData();
		std::string toWrite = toSave.getAsString();
		std::ofstream file;
		file.open("savedata/" + std::string(savefileName) + ".xml");
		file << toWrite;
		file.close();
	}
	// Postal codes ui
	if (this->currentState == UiState::EditingPostalCodes) {
		if (ImGui::CollapsingHeader("Postal Codes")) {
			for (long long id : PostalCodeDatabase::get()->getAllIds()) {
				PostalCode code = PostalCodeDatabase::get()->getPostalCode(id);
				if (this->pCode == id) {
					sprintf_s(buf, "%s", code.getName().c_str());
					ImGui::InputText("Name:", buf, 200);
					sf::Color color = code.getColor();
					float c[3] = { color.r / 255.0f, color.g / 255.0f, color.b / 255.0f };
					ImGui::ColorEdit3("", c);
					color = sf::Color(c[0] * 255.0f, c[1] * 255.0f, c[2] * 255.0f, 100.0f);
					code.setName(buf);
					code.setColor(color);
					PostalCodeDatabase::get()->setPostalCode(id, code);
				}
				else {
					ImGui::Text(code.getName().c_str());
					ImGui::SameLine();
					if (ImGui::Button("Edit")) {
						this->pCode = id;
					}
				}
			}
		}
		if (ImGui::Button("New code")) {
			PostalCodeDatabase::get()->createPostalCode(
				"Postal Code " + std::to_string(PostalCodeDatabase::get()->getAllIds().size())
			);
		}

		if (ImGui::Button("Done")) {
			this->changeState(UiState::Default);
		}
		// Change postal code if the mouse is clicked
		if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
			sf::Vector2i pos = this->getHoveredTile();
			this->game->getGameMap()->setCodeForTile((size_t)pos.x, (size_t)pos.y, this->pCode);
		}
	}
	else {
		if (ImGui::Button("Edit Postal Codes")) {
			this->changeState(UiState::EditingPostalCodes);
		}
	}

	ImGui::End();
	ImGui::Begin("Visualize");
	this->showingCityLimits = checkbox("Show city limits", this->showingCityLimits);
	if (this->showingCityLimits) {
		for (auto it = this->cityLimitColors.begin(); it != this->cityLimitColors.end(); it++) {
			sprintf_s(buf, "%llu", it->first);
			sf::Color color = it->second;
			float c[3] = { color.r / 256.0f, color.g / 256.0f, color.b / 256.0f };
			ImGui::ColorEdit3(buf, c);
			it->second = sf::Color(256.0f * c[0], 256.0f * c[1], 256.0f * c[2]);
		}
	}
	this->showingResidencePaths = checkbox("Show Residence Paths", this->showingResidencePaths);
	ImGui::End();

	ImGui::Begin("Build/Delete");
	if (ImGui::Button("Open road building")) {
		this->roadWindowOpen = true;
	}
	if (ImGui::Button("Open railway building")) {
		this->railsWindowOpen = true;
	}
	if (ImGui::Button("Open airplane road building")) {
		this->airplaneRoadWindowOpen = true;
	}
	if (ImGui::CollapsingHeader("Buildings")) {
		for (auto kv : Construction::recipes) {
			if (this->game->getTechTree()->getTechUnlocked(kv.second.getRequiredTech())) {
				if (ImGui::Button(kv.second.getDisplayName().c_str())) {
					this->recipe = kv.second;
					this->changeState(UiState::BuildingEntity);
				}
			}
		}
	}
	// Delete/Stop deleting buttons
	if (this->currentState == UiState::Deleting || this->currentState == UiState::DeletingRailwaysAndRoads) {
		if (ImGui::Button("Stop Deleting")) {
			this->currentState = UiState::Default;
		}
	}
	else {
		if (ImGui::Button("Delete")) {
			this->currentState = UiState::Deleting;
		}
		if (ImGui::Button("Delete Roads/Railways")) {
			this->currentState = UiState::DeletingRailwaysAndRoads;
		}
	}
	ImGui::End();
	ImGui::Begin("Debug Info");
	// Print FPS
	sprintf_s(buf, "FPS: %f", 1.0f / this->deltaClock.restart().asSeconds());
	ImGui::Text(buf);
	// Print mouse position
	sf::Vector3f mousePos = this->game->getGroundMousePosition();
	sf::Vector2i tile = this->getHoveredTile();
	sprintf_s(buf, "Mouse position is (%.2f,%.2f,%.2f), tile is (%d, %d)", mousePos.x, mousePos.y, mousePos.z, tile.x, tile.y);
	ImGui::Text(buf);
	sprintf_s(buf, "Heights around tile are %u, %u, %u, %u",
		this->game->getGameMap()->getPointHeight(tile),
		this->game->getGameMap()->getPointHeight(tile + sf::Vector2i(1, 0)),
		this->game->getGameMap()->getPointHeight(tile + sf::Vector2i(1, 1)),
		this->game->getGameMap()->getPointHeight(tile + sf::Vector2i(0, 1))
	);
	ImGui::Text(buf);
	ImGui::End();
	// Draw the various windows to build things
	if (this->roadWindowOpen) {
		ImGui::Begin("Road building");
		if (ImGui::Button("Roads")) {
			this->changeState(UiState::BuildingRoad);
		}
		if (ImGui::Button("Tunnel")) {
			this->changeState(UiState::BuildingTunnel);
			this->tunnelType = TransitType::Car;
		}
		ImGui::End();
	}
	if (this->railsWindowOpen) {
		ImGui::Begin("Railway Building");
		if (ImGui::Button("Rails")) {
			this->changeState(UiState::BuildingRailTracks);
		}
		if (ImGui::Button("Tunnel")) {
			this->changeState(UiState::BuildingTunnel);
			this->tunnelType = TransitType::Train;
		}
		this->isStation = checkbox("Station: ", this->isStation);
		this->isOneWay = checkbox("One way rail: ", this->isOneWay);
		ImGui::End();
	}
	if (this->airplaneRoadWindowOpen) {
		ImGui::Begin("Airplane road Building");
		if (ImGui::Button("Runways")) {
			this->changeState(UiState::BuildingAirplaneRoad);
		}
		this->isRunway = checkbox("Runway: ", this->isRunway);
		ImGui::End();
	}
	// Draw the Tech tree window
	this->game->getTechTree()->update();
	// Remove deleted entities
	this->toRenderUi.erase(std::remove_if(this->toRenderUi.begin(), this->toRenderUi.end(), [](std::weak_ptr<Entity> a) {
		if (a.lock())
			return false;
		return true;
		}), this->toRenderUi.end());
	// Render Ui for entities
	for (auto it = this->toRenderUi.begin(); it != this->toRenderUi.end(); it++) {
		if (it->lock() && it->lock()->controller) {
			it->lock()->controller->renderUi();
		}
	}

	// Cheats menu for debugging and other things
	ImGui::Begin("Cheats");
	game->setEnforceBudget(checkbox("Enforce Budget: ", game->getEnforceBudget()));
	ImGui::End();
}

void UiHandler::render(sf::RenderWindow* w) {
	// Variables in switch statement
	bool isValid;
	sf::Vector2f mousePos = this->game->getMousePosition();
	switch (this->currentState) {
	case UiState::BuildingEntity: {
		// Draw the recipe being build
		if (!this->recipe) {
			throw std::runtime_error("Tried to draw a construction sprite with no recipe!");
		}
		isValid = this->recipe.value().isValid(this->game, sf::Vector3f(mousePos.x, mousePos.y, 0), this->currentRotation);
		this->recipe.value().renderConstructionSprite(
			this->game,
			sf::Vector3f(mousePos.x, mousePos.y, 0),
			this->currentRotation,
			w);
		break;
	}
	case UiState::BuildingRoad:
	case UiState::BuildingAirplaneRoad: {
		sf::Color c = sf::Color::White;
		if (this->currentState == UiState::BuildingAirplaneRoad) {
			c = this->isRunway ? sf::Color::Blue : sf::Color::Red;
		}
		c.a = 100;
		if (this->startLocation.has_value()) {
			auto toBuild = this->getRoadsToBuild(this->startLocation.value(), sf::Vector2f(this->getHoveredTile()));
			for (auto p : toBuild) {
				this->game->getGameMap()->renderRoad(p.first, p.second, w, c);
			}
		}
	}
	case UiState::BuildingRailTracks: {
		// Draw a line
		sf::Vector3f m = this->game->getGroundMousePosition();
		sf::Vector2f mousePos(m.x, m.y);
		if (this->startLocation.has_value()) {
			auto rails = this->getRailwaysToBuild(this->startLocation.value(), mousePos);
			for (auto it = rails.begin(); it != rails.end(); it++) {
				this->game->getGameMap()->renderRailway(it->first, it->second, w, sf::Color::White);
			}
		}
		else {
			auto rails = this->getRailwaysToBuild(sf::Vector2f(this->getHoveredTile()) + sf::Vector2f(0.5f, 0.5f), mousePos);
			if (rails.size() > 0) {
				this->game->getGameMap()->renderRailway(rails.front().first, rails.front().second, w, sf::Color::White);
			}
		}
		break;
	}
	case UiState::EditingPostalCodes: {
		// Draw all the postal codes
		GameMap* gMap = this->game->getGameMap();
		for (size_t x = 0; x < gMap->MAP_WIDTH; x++) {
			for (size_t y = 0; y < gMap->MAP_HEIGHT; y++) {
				Tile t = gMap->getTileAt(x, y);
				PostalCode code = PostalCodeDatabase::get()->getPostalCode(t.postalCode);
				// Get the tile outline
				sf::VertexArray toRender = this->getDrawableTile(
					sf::Vector2i((int)x, (int)y),
					sf::PrimitiveType::Quads,
					code.getColor());
				// Draw the tile
				w->draw(toRender);
			}
		}
		break;
	}
	
	case UiState::Terraforming: {
		sf::Vector2i point = this->getHoveredPoint();
		unsigned int height = this->game->getGameMap()->getPointHeight(point.x, point.y);
		sf::Vector3f p(point.x, point.y, height);
		sf::CircleShape circle;
		circle.setPosition(this->game->worldToScreenPos(p));
		circle.setFillColor(sf::Color::Red);
		circle.setRadius(2.0f);
		w->draw(circle);
		break;
	}
	case UiState::BuildingTunnel:
		sf::Vector2i entrance = this->getHoveredTile();
		std::optional<sf::Vector2i> exit = this->getTunnelExitForTile(entrance);
		if (exit.has_value()) {
			sf::Vector3f pOne(entrance.x + 0.5f, entrance.y + 0.5f, this->game->getGameMap()->getHeightAt(sf::Vector2f(entrance) + sf::Vector2f(0.5, 0.5)));
			sf::Vector3f pTwo(exit.value().x + 0.5f, exit.value().y + 0.5f, this->game->getGameMap()->getHeightAt(sf::Vector2f(exit.value()) + sf::Vector2f(0.5, 0.5)));
			sf::VertexArray line(sf::PrimitiveType::Lines, 2);
			line[0] = sf::Vertex(this->game->worldToScreenPos(pOne), sf::Color::Red);
			line[1] = sf::Vertex(this->game->worldToScreenPos(pTwo), sf::Color::Blue);
			w->draw(line);
		}
		break;
	}
	if (this->showingCityLimits) {
		GameMap* gMap = this->game->getGameMap();
		for (size_t x = 0; x < gMap->MAP_WIDTH; x++) {
			for (size_t y = 0; y < gMap->MAP_HEIGHT; y++) {
				Tile t = gMap->getTileAt(x, y);
				// Insert color if does not exist
				if (this->cityLimitColors.count(t.cityId) == 0) {
					this->cityLimitColors.insert({ t.cityId, sf::Color(rand() % 256, rand() % 256, rand() % 256) });
				}
				sf::Color color = this->cityLimitColors.find(t.cityId)->second;
				color.a = 100.0f;
				sf::VertexArray toRender = this->getDrawableTile(
					sf::Vector2i(x, y),
					sf::PrimitiveType::Quads,
					color
				);
				w->draw(toRender);
			}
		}
	}
	if (this->showingResidencePaths) {
			GameMap* gMap = this->game->getGameMap();
			sf::Vector2i t = this->getHoveredTile();
			Tile tile = gMap->getTileAt(t);
			if (tile.building.lock() && tile.building.lock()->tag == EntityTag::Residence) {
				std::vector<sf::Vector2i> dests = std::dynamic_pointer_cast<ResidenceController>(tile.building.lock()->controller)->getDestinations();
				for (sf::Vector2i d : dests) {
					sf::VertexArray toRender(sf::PrimitiveType::LinesStrip, 2);
					toRender[0].position = this->game->worldToScreenPos(Utils::toVector3f(t));
					toRender[0].color = sf::Color::Red;
					toRender[1].position = this->game->worldToScreenPos(Utils::toVector3f(d));
					toRender[1].color = sf::Color::Green;
					w->draw(toRender);
				}
			}
	}

	// Outline the sprite currently being hovered
	sf::VertexArray vArr = getDrawableTile(this->getHoveredTile(), sf::PrimitiveType::LinesStrip, sf::Color::White);
	w->draw(vArr);

	for (std::vector<RoutePoint> path : this->pathsToDraw) {
		if (path.empty())
			continue;
		RoutePoint prev = path.front();
		for (auto it = path.begin() + 1; it != path.end(); it++) {
			sf::Vector3f prevPos = prev.pos;
			sf::Vector3f newPos = it->pos;
			sf::Vector3f diff = newPos - prevPos;
			IsoRotation rot = IsoRotation::fromUnitVector(sf::Vector2f(diff.x, diff.y));
			drawArrow(w, sf::Vector2i(prevPos.x, prevPos.y), rot, true, Utils::getTimeColor(it->expectedTime));
			prev = *it;
		}
	}

	// Render the current mouse position
	sf::CircleShape c;
	c.setFillColor(sf::Color::Red);
	c.setRadius(5.0f);
	c.setOrigin(sf::Vector2f(1, 1) * c.getRadius());
	c.setPosition(this->game->worldToScreenPos(this->game->getGroundMousePosition()));
	w->draw(c);

	this->pathsToDraw = {};
}

void UiHandler::drawArrow(sf::RenderWindow* window, sf::Vector2i tile, IsoRotation rot, bool isOutgoing, std::optional<sf::Color> color) {
	// Load the arrow sprite
	std::string sprName = "arrow_";
	if (color.has_value()) {
		sprName = "whiteArrows/arrow_";
	}
	switch ((rot + game->getRotation()).getRotation()) {
	case IsoRotation::NORTH:
		sprName += "N";
		break;
	case IsoRotation::EAST:
		sprName += "E";
		break;
	case IsoRotation::SOUTH:
		sprName += "S";
		break;
	case IsoRotation::WEST:
		sprName += "W";
		break;
	default:
		// Oh no
		throw std::runtime_error("Invalid IsoRotation in UiHander::drawArrrow!");
	}
	sprName += ".png";
	sf::Sprite arrow = ResourceLoader::get()->getIndividualSprite(sprName);
	// Set origin to bottom center
	arrow.setOrigin(
		arrow.getGlobalBounds().width / 2,
		arrow.getGlobalBounds().height / 2
	);
	// If not outgoing, position itself to point out of the tile
	if (!isOutgoing)
		rot = rot.getReverse();
	arrow.setPosition(
		this->game->worldToScreenPos(sf::Vector3f((float)tile.x, (float)tile.y, this->game->getGameMap()->getHeightAt(sf::Vector2f(tile)))
			+ sf::Vector3f(0.5f, 0.5f, 0)
			+ rot.getUnitVector3D() / 2.0f)
	);
	arrow.setScale(0.5f, 0.5f);
	if (color.has_value()) {
		arrow.setColor(color.value());
	}
	window->draw(arrow);
}
void UiHandler::changeState(UiState state) {
	this->currentState = state;
}

sf::VertexArray UiHandler::getDrawableTile(sf::Vector2i pos, sf::PrimitiveType t, sf::Color c) {
	sf::VertexArray vArr(t, 5);
	sf::Vector3f fPos((float)pos.x, (float)pos.y, 0);
	Game* g = this->game;
	for (size_t x = 0; x < 2; x++) {
		for (size_t y = 0; y < 2; y++) {
			vArr[2 * x + y] = sf::Vertex(g->worldToScreenPos(sf::Vector3f(pos.x + x, pos.y + y, this->game->getGameMap()->getPointHeight(pos.x + x, pos.y + y))), c);
		}
	}
	vArr[4] = vArr[0];
	return vArr;
}

IsoRotation UiHandler::chooseDirection(const char* label, IsoRotation def) {
	IsoRotation toReturn = def;
	char preview[200];
	switch (def.getRotation()) {
	case IsoRotation::NORTH:
		strncpy_s(preview, "N", 2);
		break;
	case IsoRotation::EAST:
		strncpy_s(preview, "E", 2);
		break;
	case IsoRotation::SOUTH:
		strncpy_s(preview, "S", 2);
		break;
	case IsoRotation::WEST:
		strncpy_s(preview, "W", 2);
		break;
	}
	if (ImGui::BeginCombo(label, preview)) {
		if (ImGui::Selectable("N")) {
			toReturn = IsoRotation::NORTH;
		}
		if (ImGui::Selectable("E")) {
			toReturn = IsoRotation::EAST;
		}
		if (ImGui::Selectable("S")) {
			toReturn = IsoRotation::SOUTH;
		}
		if (ImGui::Selectable("W")) {
			toReturn = IsoRotation::WEST;
		}
		ImGui::EndCombo();
	}
	return toReturn;
}

hour_t UiHandler::chooseHour(const char* label, hour_t hour) {
	char buf[200];
	sprintf_s(buf, "%d:00", hour);
	if (ImGui::BeginCombo(label, buf)) {
		for (size_t i = 0; i < 24; i++) {
			sprintf_s(buf, "%d:00", i);
			if (ImGui::Selectable(buf, i == hour)) {
				hour = (hour_t)i;
			}
		}
		ImGui::EndCombo();
	}
	return hour;
}

void UiHandler::addPathToDraw(std::vector<RoutePoint> path) {
	this->pathsToDraw.push_back(path);
}

void UiHandler::setSavefileName(const char name[200]) {
	strncpy_s(this->savefileName, name, 200);
}

std::optional<IsoRotation> UiHandler::getTileSlopeDirection(sf::Vector2i tile) {
	GameMap* gMap = this->game->getGameMap();
	unsigned int topLeft = gMap->getPointHeight(tile);
	unsigned int topRight = gMap->getPointHeight(tile + sf::Vector2i(1, 0));
	unsigned int botLeft = gMap->getPointHeight(tile + sf::Vector2i(0, 1));
	unsigned int botRight = gMap->getPointHeight(tile + sf::Vector2i(1, 1));
	if (topLeft == topRight && botLeft == botRight && topLeft != botLeft) {
		return topLeft > botLeft ? IsoRotation::SOUTH : IsoRotation::NORTH;
	}
	else if (topLeft == botLeft && topRight == botRight && topLeft != topRight) {
		return topLeft > topRight ? IsoRotation::EAST : IsoRotation::WEST;
	}
	return {};
}
std::optional<sf::Vector2i> UiHandler::getTunnelExitForTile(sf::Vector2i tile) {
	auto dir = this->getTileSlopeDirection(tile);
	if (dir.has_value()) {
		unsigned int baseHeight = this->getBottomSlopeHeight(tile).value();
		IsoRotation unit = dir.value().getReverse();
		for (sf::Vector2i t = tile + sf::Vector2i(unit.getUnitVector()); this->game->getGameMap()->getTileAt(t).type != TileType::OffMap; t += sf::Vector2i(unit.getUnitVector())) {
			if (this->getTileSlopeDirection(t).value_or(IsoRotation::SOUTH_EAST) == unit && this->getBottomSlopeHeight(t).value_or(10000) == baseHeight) {
				return t;
			}
		}
	}
	return {};
}

std::optional<unsigned int> UiHandler::getBottomSlopeHeight(sf::Vector2i t) {
	auto d = this->getTileSlopeDirection(t);
	if (!d.has_value()) {
		return {};
	}
	IsoRotation dir = d.value();
	if (dir == IsoRotation::SOUTH)
		return this->game->getGameMap()->getPointHeight(t + sf::Vector2i(0, 1));
	else if (dir == IsoRotation::EAST)
		return this->game->getGameMap()->getPointHeight(t + sf::Vector2i(1, 0));
	// South and East can both use the top-left corner
	return this->game->getGameMap()->getPointHeight(t);
}

std::vector<std::pair<sf::Vector2i, Road>> UiHandler::getRoadsToBuild(sf::Vector2f from, sf::Vector2f to) {
	// Get the centers to correctly determine if the road is going north-south or east-west
	sf::Vector2f centerFrom = from + sf::Vector2f(0.5, 0.5);
	sf::Vector2f centerTo = to + sf::Vector2f(0.5, 0.5);
	sf::Vector2f diff = centerTo - centerFrom;
	IsoRotation rot;
	if (diff.x > diff.y) {
		if (-diff.x > diff.y) {
			rot = IsoRotation::NORTH;
		}
		else {
			rot = IsoRotation::EAST;
		}
	}
	else {
		if (-diff.x > diff.y) {
			rot = IsoRotation::WEST;
		}
		else {
			rot = IsoRotation::SOUTH;
		}
	}
	bool isVert = (rot == IsoRotation::NORTH || rot == IsoRotation::SOUTH);
	// Get the to positions snapped to either being completely horizontal or completely vertical
	sf::Vector2f snappedTo = isVert ? sf::Vector2f(from.x, to.y) : sf::Vector2f(to.x, from.y);
	// Add the first road
	std::vector<std::pair<sf::Vector2i, Road>> toReturn = {
		{ sf::Vector2i(from), Road(rot) }
	};
	// Add the roads inbetween
	size_t length = Utils::getVectorDistance(snappedTo, from);
	Road r = isVert ? Road(true, false, true, false) : Road(false, true, false, true);
	for (size_t i = 1; i < length; i++) {
		toReturn.push_back({ sf::Vector2i(from + rot.getUnitVector() * (float)i), r });
	}
	// Add the last road
	toReturn.push_back({ sf::Vector2i(snappedTo), Road(rot.getReverse()) });
	return toReturn;
}

std::vector<std::pair<sf::Vector2i, Railway>> UiHandler::getRailwaysToBuild(sf::Vector2f from, sf::Vector2f to) {
	sf::Vector2f diff = to - from;
	IsoRotation rot = IsoRotation::fromUnitVector(diff);
	sf::Vector2f unit;
	sf::Vector2f offset;
	sf::Vector2f startTile = sf::Vector2f(sf::Vector2i(from)) + sf::Vector2f(0.5f, 0.5f);
	sf::Vector2f temp(startTile - to);
	switch (rot.getRotation()) {
	case IsoRotation::NORTH:
	case IsoRotation::EAST:
	case IsoRotation::SOUTH:
	case IsoRotation::WEST:
		unit = rot.getUnitVector();
		offset = sf::Vector2f(0.5f, 0.5f) + rot.getReverse().getUnitVector() / 2.0f;
		break;
	case IsoRotation::NORTH_EAST:
		unit = sf::Vector2f(0.5, -0.5);
		offset = diff.y / diff.x > -1 ? sf::Vector2f(0.5f, 1) : sf::Vector2f(0, 0.5f);
		break;
	case IsoRotation::SOUTH_EAST:
		unit = sf::Vector2f(0.5, 0.5);
		offset = diff.y / diff.x > 1 ? sf::Vector2f(0, 0.5f) : sf::Vector2f(0.5f, 0);
		break;
	case IsoRotation::SOUTH_WEST:
		unit = sf::Vector2f(-0.5, 0.5);
		offset = diff.y / diff.x > -1 ? sf::Vector2f(0.5f, 0) : sf::Vector2f(1, 0.5f);
		break;
	case IsoRotation::NORTH_WEST:
		unit = sf::Vector2f(-0.5, -0.5);
		offset = diff.y / diff.x > 1 ? sf::Vector2f(1, 0.5f) : sf::Vector2f(0.5, 1);
		break;
	}
	sf::Vector2f start2d = sf::Vector2f(sf::Vector2i(from)) + offset;
	// Round the length
	size_t numSegments = (size_t)std::max((int)ceil(sqrtf(diff.x * diff.x + diff.y * diff.y)), 1);
	sf::Vector2f start(start2d.x, start2d.y);
	std::vector<std::pair<sf::Vector2i, Railway>> toReturn;
	for (size_t i = 1; i < numSegments + 1; i++) {
		sf::Vector2f end = start + unit;
		sf::Vector2i tile(std::min(floor(start.x), floor(end.x)), std::min(floor(start.y), floor(end.y)));
		sf::Vector2f center(tile.x + 0.5f, tile.y + 0.5f);
		IsoRotation from = IsoRotation::fromUnitVector(start - center);
		IsoRotation to = IsoRotation::fromUnitVector(end - center);
		if (from.getIsOrdinal() || to.getIsOrdinal()) {
			auto x = 0;
		}
		toReturn.push_back({ tile, Railway(from, to, this->isOneWay) });
		start = end;
	}
	return toReturn;
}

bool UiHandler::checkbox(const char* label, bool value) {
	bool v = value;
	ImGui::Checkbox(label, &v);
	return v;
}