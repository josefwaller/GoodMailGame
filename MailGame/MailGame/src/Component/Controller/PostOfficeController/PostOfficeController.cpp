#include "PostOfficeController.h"
#include "Entity/Entity.h"
#include "Component/Transform/Transform.h"
#include <imgui.h>

void PostOfficeController::update(float delta) {
	if (this->getEntity()->transform->isOnScreen()) {
		ImGui::Begin("Post Office");
		for (auto it = this->routes.begin(); it != this->routes.end(); it++) {
			ImGui::Button("Route!");
		}
		if (ImGui::Button("Create Route")) {
			this->routes.push_back(MailTruckRoute(true, 0));
		}
		ImGui::End();
	}
}
