#include "PostOfficeController.h"
#include "Entity/Entity.h"
#include "Component/Transform/Transform.h"
#include <string>
#include <imgui.h>

void PostOfficeController::update(float delta) {
	if (this->getEntity()->transform->isOnScreen()) {
		ImGui::PushID(this->getEntity()->getId());
		ImGui::Begin("Post Office");
		for (auto it = this->routes.begin(); it != this->routes.end(); it++) {
			// Get the route index
			size_t index = (size_t)(it - this->routes.begin());
			// Get the ID as a string, for labels
			ImGui::PushID(it->id);
			// Create a collapsing header
			std::string routeName = "Route " + std::to_string(it->id);
			if (ImGui::CollapsingHeader(routeName.c_str())) {
				// Add dropdown for time
				if (ImGui::BeginCombo("Departure Time", std::to_string(it->departTime).c_str())) {
					for (size_t i = 0; i < 24; i++) {
						if (ImGui::Selectable(std::to_string(i).c_str())) {
							this->setRouteTime(index, (int)i);
						}
						if (it->departTime == i) {
							ImGui::SetItemDefaultFocus();
						}
					}
					ImGui::EndCombo();
				}
				if (ImGui::Button("Delete")) {
					this->deleteRoute(index);
				}
			}
			ImGui::PopID();
		}
		if (ImGui::Button("Create Route")) {
			this->routes.push_back(MailTruckRoute(true, 0));
		}
		ImGui::End();
		ImGui::PopID();
	}
	// Delete any routes that are flagged for deletion
	for (int i = (int)(routes.size() - 1); i >= 0; i--) {
		// Check if the route should be deleted
		if (std::find(this->routesToDelete.begin(), this->routesToDelete.end(), i) != this->routesToDelete.end()) {
			// Delete it
			this->routes.erase(this->routes.begin() + i);
		}
	}
	this->routesToDelete.clear();
}

void PostOfficeController::setRouteTime(size_t routeIndex, int time) {
	this->routes[routeIndex].departTime = time;
}
void PostOfficeController::setRouteType(size_t routeIndex, bool isDelivering) {
	this->routes[routeIndex].isDelivering = isDelivering;
}
void PostOfficeController::deleteRoute(size_t routeIndex) {
	routesToDelete.push_back(routeIndex);
}
