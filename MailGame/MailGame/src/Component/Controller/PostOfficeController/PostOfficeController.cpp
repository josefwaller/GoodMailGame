#include "PostOfficeController.h"
#include "Entity/Entity.h"
#include "Component/Transform/Transform.h"
#include <string>
#include <imgui.h>

void PostOfficeController::update(float delta) {
	if (this->getEntity()->transform->isOnScreen()) {
		ImGui::Begin("Post Office");
		for (auto it = this->routes.begin(); it != this->routes.end(); it++) {
			// Get teh route index
			size_t index = (size_t)(it - this->routes.begin());
			// Create a collapsing header
			std::string routeName = "Route " + std::to_string((int)(it - this->routes.begin()));
			if (ImGui::CollapsingHeader(routeName.c_str())) {
				// Add dropdown for time
				if (ImGui::BeginCombo("Departure Time", std::to_string(it->departTime).c_str())) {
					for (size_t i = 0; i < 24; i++) {
						if (ImGui::Selectable(std::to_string(i).c_str())) {
							this->setRouteTime(index, i);
						}
						if (it->departTime == i) {
							ImGui::SetItemDefaultFocus();
						}
					}
					ImGui::EndCombo();
				}
			}
		}
		if (ImGui::Button("Create Route")) {
			this->routes.push_back(MailTruckRoute(true, 0));
		}
		ImGui::End();
	}
}

void PostOfficeController::setRouteTime(size_t routeIndex, int time) {
	this->routes[routeIndex].departTime = time;
}
void PostOfficeController::setRouteType(size_t routeIndex, bool isDelivering) {
	this->routes[routeIndex].isDelivering = isDelivering;
}
