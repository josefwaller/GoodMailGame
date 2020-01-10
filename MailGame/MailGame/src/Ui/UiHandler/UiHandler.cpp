#include "UiHandler.h"
#include <imgui.h>
#include <imgui-SFML.h>
#include "Game/Game.h"

UiHandler::UiHandler(Game* g): game(g), currentState(UiState::Default) {}

bool UiHandler::handleEvent(sf::Event e) {
	if (e.type == sf::Event::MouseButtonPressed) {
		if (ImGui::GetIO().WantCaptureMouse) {
			return true;
		}
		switch(this->currentState) {
		case UiState::Building:
			sf::Vector2f mousePos = this->game->getMousePosition();
			if (this->recipe.positionIsValid(this->game, mousePos, this->currentRotation)) {
				this->game->addEntity(
					this->recipe.createFunction(this->game, mousePos, this->currentRotation)
				);
			}
			this->changeState(UiState::Default);
			return true;
		case UiState::Selecting:
		}
	} else {
		if (ImGui::GetIO().WantCaptureKeyboard) {
			return true;
		}
	}
	return false;
}

void UiHandler::selectEntity(std::function<void(std::weak_ptr<Entity>)> callback) {
	this->changeState(UiState::Selecting);
	this->selectCallback = callback;
}
void UiHandler::update() {
	ImGui::Begin("Game Controls");

	if (ImGui::Button("Rotate Camera")) {
		this->game->rotateCamera();
	}
	if (ImGui::CollapsingHeader("Build")) {
		if (ImGui::Button("Post Office")) {
			this->recipe = Construction::recipes[EntityTag::PostOffice];
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

	ImGui::End();
}

void UiHandler::render(sf::RenderWindow* w) {
	if (this->currentState == UiState::Building) {
		bool isValid = this->recipe.positionIsValid(this->game, this->game->getMousePosition(), this->currentRotation);
		w->draw(this->recipe.getRenderSprite(this->game, this->game->getMousePosition(), this->currentRotation, isValid));
	}
}

void UiHandler::changeState(UiState state) {
	this->currentState = state;
}
