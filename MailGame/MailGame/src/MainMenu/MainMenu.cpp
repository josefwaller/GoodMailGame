#include "./MainMenu.h"
#include  "App/App.h"
#include <imgui.h>

MainMenu::MainMenu(App* a) : app(a) {
}

void MainMenu::update() {
	ImGui::Begin("Main menu");

	if (ImGui::Button("New game")) {
		this->app->generateNewGame();
	}
	if (ImGui::Button("Load test.xml")) {
		this->app->loadSaveFile("savedata/test.xml");
	}
	ImGui::End();
}