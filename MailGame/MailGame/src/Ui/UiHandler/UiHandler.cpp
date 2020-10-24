#include "UiHandler.h"
#include "Entity/Entity.h"
#include "Mail/Mail.h"
#include "Component/ClickBox/ClickBox.h"
#include <imgui.h>
#include <iostream>
#include <fstream>
#include <imgui-SFML.h>
#include <SFML/Graphics/VertexArray.hpp>
#include "Game/Game.h"
#include "PostalCodeDatabase/PostalCodeDatabase.h"
#include "ResourceLoader/ResourceLoader.h"
#include "System/SaveData/SaveData.h"

UiHandler::UiHandler(Game* g): game(g), currentState(UiState::Default), recipe(),
	toBuild(IsoRotation::NORTH, IsoRotation::SOUTH) {}

bool UiHandler::handleEvent(sf::Event e) {
	sf::Vector2i tilePos;
	sf::Vector2f mousePos;
	switch (e.type) {
	case sf::Event::MouseButtonPressed:
		if (ImGui::GetIO().WantCaptureMouse) {
			return true;
		}
		switch(this->currentState) {
		case UiState::BuildingEntity:
			if (!this->recipe) {
				throw std::runtime_error("BuildingEntity but with no ConstructionRecipe!");
			}
			mousePos = this->game->getMousePosition();
			if (this->recipe.value().isValid(this->game, sf::Vector3f(mousePos.x, mousePos.y, 0), this->currentRotation)) {
				this->game->addEntity(
					this->recipe.value().buildRecipe(this->game, sf::Vector3f(mousePos.x, mousePos.y, 0), this->currentRotation)
				);
			}
			this->changeState(UiState::Default);
			return true;
		case UiState::BuildingRailTracks:
			tilePos = sf::Vector2i(this->game->getMousePosition());
			this->game->getGameMap()->addRailTrack(
				(size_t)tilePos.x,
				(size_t)tilePos.y,
				this->toBuild.from,
				this->toBuild.to,
				this->toBuildTime
			);
			break;
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
		}
	case sf::Event::KeyPressed:
		if (ImGui::GetIO().WantCaptureKeyboard) {
			return true;
		}
		switch (e.key.code) {
		// Rotate the building/track if R is pressed
		case sf::Keyboard::R:
			if (this->currentState == UiState::BuildingRailTracks) {
				this->toBuild.from++;
				this->toBuild.to++;
			}
			else if (this->currentState == UiState::BuildingEntity) {
				this->currentRotation++;
			}
			break;
		case sf::Keyboard::T:
			if (this->currentState == UiState::BuildingRailTracks) {
				// Toggle between straight and curved track
				this->toBuild.to++;
				if (this->toBuild.from == this->toBuild.to) {
					this->toBuild.to++;
				}
			}
		}
	}
	return false;
}

sf::Vector2i UiHandler::getHoveredTile() {
	sf::Vector2f mousePos = this->game->getMousePosition();
	return sf::Vector2i((int)floor(mousePos.x), (int)floor(mousePos.y));
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
	ImGui::Begin("Game Controls");
	// Print percent of letters that have been delivered
	sprintf_s(buf, "%f of letters delivered", Mail::getPercentDelivered() * 100.0f);
	ImGui::Text(buf);

	if (ImGui::Button("Rotate Camera")) {
		this->game->rotateCamera();
	}
	ImGui::Text((std::to_string(this->game->getHour()) + ":00").c_str());
	if (ImGui::Button("Toggle Paused")) {
		this->game->togglePause();
	}
	ImGui::Text((std::string("Game.time: ") + std::to_string(this->game->getTime())).c_str());
	if (ImGui::Button("Save Game")) {
		// Terrible way of temporarily doing this
		// Just save rn to file
		SaveData toSave = this->game->getSaveData();
		std::string toWrite = toSave.getAsString();
		std::ofstream file;
		file.open("savedata/test.txt");
		file << toWrite;
		file.close();
	}
	if (ImGui::CollapsingHeader("Build")) {
		if (ImGui::Button("Post Office")) {
			this->recipe = Construction::recipes.at(EntityTag::PostOffice);
			this->changeState(UiState::BuildingEntity);
		}
		if (ImGui::Button("MailBox")) {
			this->recipe = Construction::recipes.at(EntityTag::MailBox);
			this->changeState(UiState::BuildingEntity);
		}
		if (ImGui::Button("Cargo Truck Depot")) {
			this->recipe = Construction::recipes.at(EntityTag::CargoTruckDepot);
			this->changeState(UiState::BuildingEntity);
		}
		if (ImGui::Button("Train Station")) {
			this->recipe = Construction::recipes.at(EntityTag::TrainStation);
			this->changeState(UiState::BuildingEntity);
		}
		if (ImGui::Button("Airport")) {
			this->recipe = Construction::recipes.at(EntityTag::Airport);
			this->changeState(UiState::BuildingEntity);
		}
		if (ImGui::Button("RailWay")) {
			this->changeState(UiState::BuildingRailTracks);
		}
		if (this->currentState == UiState::BuildingRailTracks) {
			this->toBuild.from = this->chooseDirection("From:", this->toBuild.from);
			this->toBuild.to = this->chooseDirection("To:", this->toBuild.to);
			if (ImGui::BeginCombo("Hour", std::to_string(this->toBuildTime).c_str())) {
				for (size_t i = 0; i < 24; i++) {
					if (ImGui::Selectable(std::to_string(i).c_str(), i == this->toBuildTime)) {
						this->toBuildTime = i;
					}
				}
				ImGui::EndCombo();
			}
			if (ImGui::Button("Cancel")) {
				this->changeState(UiState::Default);
			}
		}

		if (this->currentState == UiState::BuildingEntity) {
			if (ImGui::Button("Rotate")) {
				this->currentRotation++;
			}
			if (ImGui::Button("Cancel")) {
				this->changeState(UiState::Default);
			}
		}
	}
	// Print mouse position
	sf::Vector2f mousePos = this->game->getMousePosition();
	sf::Vector2i tile = this->getHoveredTile();
	sprintf_s(buf, "Mouse position is (%.2f,%.2f), tile is (%d, %d)", mousePos.x, mousePos.y, tile.x, tile.y);
	ImGui::Text(buf);

	// Postal codes ui
	if (this->currentState == UiState::EditingPostalCodes) {
		if (ImGui::CollapsingHeader("Postal Codes")) {
			for (long long id : PostalCodeDatabase::get()->getAllIds()) {
				sprintf_s(buf, "%lu", id);
				if (ImGui::Button(buf)) {
					this->pCode = id;
				}
			}
		}
		if (ImGui::Button("New code")) {
			PostalCodeDatabase::get()->createPostalCode({
				sf::Color(rand() % 256, rand() % 256, rand() % 256, 100)
			});
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
}

void UiHandler::render(sf::RenderWindow* w) {
	// Variables in switch statement
	bool isValid;
	sf::Vector2f mousePos;
	switch (this->currentState) {
	case UiState::BuildingEntity:
		// Draw the recipe being build
		if (!this->recipe) {
			throw std::runtime_error("Tried to draw a construction sprite with no recipe!");
		}
		mousePos = this->game->getMousePosition();
		isValid = this->recipe.value().isValid(this->game, sf::Vector3f(mousePos.x, mousePos.y, 0), this->currentRotation);
		this->recipe.value().renderConstructionSprite(
			this->game,
			sf::Vector3f(mousePos.x, mousePos.y, 0),
			this->currentRotation,
			w);
		break;
	case UiState::BuildingRailTracks:
		// Draw 2 lines that correspond with the direction
		this->drawArrow(w, this->getHoveredTile(), this->toBuild.from + 2, false);
		this->drawArrow(w, this->getHoveredTile(), this->toBuild.to, true);
		break;
	case UiState::EditingPostalCodes:
		// Draw all the postal codes
		GameMap* gMap = this->game->getGameMap();
		for (size_t x = 0; x < gMap->MAP_WIDTH; x++) {
			for (size_t y = 0; y < gMap->MAP_HEIGHT; y++) {
				Tile t = gMap->getTileAt(x, y);
				PostalCodeDatabase::CodeInfo code = PostalCodeDatabase::get()->getPostalCode(t.postalCode);
				// Get the tile outline
				sf::VertexArray toRender = this->getDrawableTile(
					sf::Vector2i((int)x, (int)y),
					sf::PrimitiveType::Quads,
					code.color);
				// Draw the tile
				w->draw(toRender);
			}
		}
		break;
	}
	// Outline the sprite currently being hovered
	sf::VertexArray vArr = getDrawableTile(this->getHoveredTile(), sf::PrimitiveType::LinesStrip, sf::Color::White);
	w->draw(vArr);
}

void UiHandler::drawArrow(sf::RenderWindow* window, sf::Vector2i tile, IsoRotation rot, bool isOutgoing) {
	// Load the arrow sprite
	std::string sprName = "arrow_";
	switch((rot + game->getRotation()).getRotation()) {
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
		rot = rot + 2;
	arrow.setPosition(
		this->game->worldToScreenPos(sf::Vector3f((float)tile.x, (float)tile.y, 0)
			+ sf::Vector3f(0.5f, 0.5f, 0)
			+ rot.getUnitVector3D() / 2.0f)
	);
	arrow.setScale(0.5f, 0.5f);
	window->draw(arrow);
}
void UiHandler::changeState(UiState state) {
	this->currentState = state;
}

sf::VertexArray UiHandler::getDrawableTile(sf::Vector2i pos, sf::PrimitiveType t, sf::Color c) {
	sf::VertexArray vArr(t, 5);
	sf::Vector3f fPos((float)pos.x, (float)pos.y, 0);
	Game* g = this->game;
	vArr[0] = sf::Vertex(g->worldToScreenPos(fPos), c);
	vArr[1] = sf::Vertex(g->worldToScreenPos(fPos + sf::Vector3f(1.0f, 0, 0)), c);
	vArr[2] = sf::Vertex(g->worldToScreenPos(fPos + sf::Vector3f(1.0f, 1.0f, 0)), c);
	vArr[3] = sf::Vertex(g->worldToScreenPos(fPos + sf::Vector3f(0, 1.0f, 0)), c);
	vArr[4] = vArr[0];
	return vArr;
}

IsoRotation UiHandler::chooseDirection(const char* label, IsoRotation def) {
	IsoRotation toReturn = def;
	char* preview = std::vector<char*>({ "N", "E", "S", "W" })[def.getRotation()];
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
