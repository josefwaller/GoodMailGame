#include "UiHandler.h"
#include <imgui.h>
#include <imgui-SFML.h>
#include "Game/Game.h"

UiHandler::UiHandler(Game* g): game(g) {}

bool UiHandler::handleEvent(sf::Event e) {
	if (e.type == sf::Event::MouseButtonPressed)
		return !ImGui::GetIO().WantCaptureMouse;
	else
		return !ImGui::GetIO().WantCaptureKeyboard;
}

void UiHandler::update() {
	ImGui::Begin("Game Controls");

	if (ImGui::Button("Rotate Camera")) {
		this->game->rotateCamera();
	}

	ImGui::End();
}

void UiHandler::render(sf::RenderWindow* w) {

}