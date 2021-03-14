#include "./MainMenu.h"
#include  "App/App.h"
#include <imgui.h>
#include <filesystem>

MainMenu::MainMenu(App* a) : app(a) {
}

void MainMenu::update() {
	ImGui::Begin("Main menu");

	if (ImGui::Button("New game")) {
		this->app->generateNewGame();
	}
	std::string path = "savedata/";
	std::string toLoad = "";
	for (auto entry : std::filesystem::directory_iterator(path)) {
		char path[200];
		strncpy_s(path, entry.path().stem().string().c_str(), 200);
		if (ImGui::Button(path)) {
			toLoad = path;
		}
	}
	if (!toLoad.empty()) {
		this->app->loadSaveFile(toLoad);
	}
	ImGui::End();
}