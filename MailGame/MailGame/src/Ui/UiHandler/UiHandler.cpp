#include "UiHandler.h"
#include "Entity/Entity.h"
#include "Component/ClickBox/ClickBox.h"
#include <imgui.h>
#include <imgui-SFML.h>
#include <SFML/Graphics/VertexArray.hpp>
#include "Game/Game.h"

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

	ImGui::End();
}

void UiHandler::render(sf::RenderWindow* w) {
	if (this->currentState == UiState::Building) {
		if (!this->recipe) {
			throw std::runtime_error("Tried to draw a construction sprite with no recipe!");
		}
		bool isValid = this->recipe.value().isValid(this->game, this->game->getMousePosition(), this->currentRotation);
		this->recipe.value().renderConstructionSprite(
			this->game,
			this->game->getMousePosition(),
			this->currentRotation,
			w);
	}
	// Outline the sprite currently being hovered
	sf::VertexArray vArr(sf::LinesStrip, 5);
	sf::Vector2f pos(this->getHoveredTile());
	Game* g = this->game;
	vArr[0] = sf::Vertex(g->worldToScreenPos(pos), sf::Color::White);
	vArr[1] = sf::Vertex(g->worldToScreenPos(pos + sf::Vector2f(1.0f, 0)), sf::Color::White);
	vArr[2] = sf::Vertex(g->worldToScreenPos(pos + sf::Vector2f(1.0f, 1.0f)), sf::Color::White);
	vArr[3] = sf::Vertex(g->worldToScreenPos(pos + sf::Vector2f(0, 1.0f)), sf::Color::White);
	vArr[4] = vArr[0];
	w->draw(vArr);
}

void UiHandler::changeState(UiState state) {
	this->currentState = state;
}
