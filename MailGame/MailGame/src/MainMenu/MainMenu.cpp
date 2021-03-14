#include "./MainMenu.h"
#include  "App/App.h"
#include <imgui.h>
#include <filesystem>

MainMenu::MainMenu(App* a) : app(a), isSure(false) {
}

void MainMenu::update() {
	ImGui::Begin("Main menu");

	if (ImGui::Button("New game")) {
		this->app->generateNewGame();
	}
	std::string path = "savedata/";
	std::string toLoad = "";
	for (auto entry : std::filesystem::directory_iterator(path)) {
		char id[200];
		strncpy_s(id, entry.path().string().c_str(), 200);
		ImGui::PushID(id);
		char path[200];
		strncpy_s(path, entry.path().stem().string().c_str(), 200);
		if (ImGui::Button(path)) {
			toLoad = path;
		}
		if (strncmp(path, this->isDeleting.c_str(), 200) == 0) {
			ImGui::SameLine();
			ImGui::Text("Are you sure?");
			ImGui::SameLine();
			if (ImGui::Button("Yes")) {
				this->isSure = true;
			}
			ImGui::SameLine();
			if (ImGui::Button("No")) {
				this->isDeleting = "";
			}
		}
		else {
			ImGui::SameLine();
			if (ImGui::Button("Delete")) {
				this->isDeleting = path;
			}
		}
		ImGui::PopID();
	}
	if (this->isSure && !this->isDeleting.empty()) {
		std::filesystem::path p(path + this->isDeleting + ".xml");
		std::filesystem::remove(p);
		this->isSure = false;
		this->isDeleting = "";
	}
	if (!toLoad.empty()) {
		this->app->loadSaveFile(toLoad);
	}
	ImGui::End();
}