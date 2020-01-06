#include "UiHandler.h"
#include <imgui.h>
#include <imgui-SFML.h>
#include "Game/Game.h"

UiHandler::UiHandler(Game* g): game(g) {}

bool UiHandler::handleEvent(sf::Event e) {
	if (e.type == sf::Event::MouseButtonPressed) {
		if (ImGui::GetIO().WantCaptureMouse) {
			return false;
		}
		if (isBuilding) {
			sf::Vector2f mousePos = this->game->getMousePosition();
			if (this->recipe.positionIsValid(this->game, mousePos, this->game->getRotation())) {
				this->game->addEntity(
					this->recipe.createFunction(this->game, mousePos, this->game->getRotation())
				);
			}
			this->isBuilding = false;
		}
	} else {
		if (ImGui::GetIO().WantCaptureKeyboard) {
			return false;
		}
	}
}

void UiHandler::update() {
	ImGui::Begin("Game Controls");

	if (ImGui::Button("Rotate Camera")) {
		this->game->rotateCamera();
	}
	if (ImGui::CollapsingHeader("Build")) {
		if (ImGui::Button("Post Office")) {
			this->recipe = Construction::recipes[EntityTag::PostOffice];
			this->isBuilding = true;
		}
	}

	ImGui::End();
}

void UiHandler::render(sf::RenderWindow* w) {

}
