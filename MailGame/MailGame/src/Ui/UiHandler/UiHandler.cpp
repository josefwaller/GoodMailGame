#include "UiHandler.h"
#include "Entity/Entity.h"
#include "Component/ClickBox/ClickBox.h"
#include <imgui.h>
#include <imgui-SFML.h>
#include <SFML/Graphics/VertexArray.hpp>
#include "Game/Game.h"
#include "PostalCodeDatabase/PostalCodeDatabase.h"

UiHandler::UiHandler(Game* g): game(g), currentState(UiState::Default), recipe() {}

bool UiHandler::handleEvent(sf::Event e) {
	if (e.type == sf::Event::MouseButtonPressed) {
		if (ImGui::GetIO().WantCaptureMouse) {
			return true;
		}
		sf::Vector2f mousePos;
		switch(this->currentState) {
		case UiState::Building:
			if (!this->recipe) {
				throw std::runtime_error("Building but with no ConstructionRecipe!");
			}
			mousePos = this->game->getMousePosition();
			if (this->recipe.value().isValid(this->game, mousePos, this->currentRotation)) {
				this->game->addEntity(
					this->recipe.value().buildRecipe(this->game, mousePos, this->currentRotation)
				);
			}
			this->changeState(UiState::Default);
			return true;
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
		case UiState::EditingPostalCodes:
			this->game->getGameMap()->setCodeForTile(this->getHoveredTile().x, this->getHoveredTile().y, this->pCode);
			break;
		}
	} else {
		if (ImGui::GetIO().WantCaptureKeyboard) {
			return true;
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
	ImGui::Begin("Game Controls");

	if (ImGui::Button("Rotate Camera")) {
		this->game->rotateCamera();
	}
	ImGui::Text((std::to_string(this->game->getTime()) + ":00").c_str());
	if (ImGui::Button("Next Hour")) {
		this->game->advanceTime();
	}
	if (ImGui::CollapsingHeader("Build")) {
		if (ImGui::Button("Post Office")) {
			this->recipe = Construction::recipes[EntityTag::PostOffice];
			this->changeState(UiState::Building);
		}
		if (ImGui::Button("MailBox")) {
			this->recipe = Construction::recipes[EntityTag::MailBox];
			this->changeState(UiState::Building);
		}
		if (ImGui::Button("Cargo Truck Depot")) {
			this->recipe = Construction::recipes[EntityTag::CargoTruckDepot];
			this->changeState(UiState::Building);
		}

		if (this->currentState == UiState::Building) {
			if (ImGui::Button("Rotate Building")) {
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
	char buf[200];
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
	switch (this->currentState) {
	case UiState::Building:
		// Draw the recipe being build
		if (!this->recipe) {
			throw std::runtime_error("Tried to draw a construction sprite with no recipe!");
		}
		isValid = this->recipe.value().isValid(this->game, this->game->getMousePosition(), this->currentRotation);
		this->recipe.value().renderConstructionSprite(
			this->game,
			this->game->getMousePosition(),
			this->currentRotation,
			w);
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

void UiHandler::changeState(UiState state) {
	this->currentState = state;
}

sf::VertexArray UiHandler::getDrawableTile(sf::Vector2i pos, sf::PrimitiveType t, sf::Color c) {
	sf::VertexArray vArr(t, 5);
	sf::Vector2f fPos(pos);
	Game* g = this->game;
	vArr[0] = sf::Vertex(g->worldToScreenPos(fPos), c);
	vArr[1] = sf::Vertex(g->worldToScreenPos(fPos + sf::Vector2f(1.0f, 0)), c);
	vArr[2] = sf::Vertex(g->worldToScreenPos(fPos + sf::Vector2f(1.0f, 1.0f)), c);
	vArr[3] = sf::Vertex(g->worldToScreenPos(fPos + sf::Vector2f(0, 1.0f)), c);
	vArr[4] = vArr[0];
	return vArr;
}
