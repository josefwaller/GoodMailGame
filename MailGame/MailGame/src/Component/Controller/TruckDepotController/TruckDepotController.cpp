#include "TruckDepotController.h"
#include <imgui.h>
#include "Entity/Entity.h"

void TruckDepotController::update(float delta) {
	ImGui::PushID((int)this->getEntity()->getId());
	ImGui::Begin("Truck Depot");

	// Show gui for each route
	for (size_t i = 0; i < this->routes.size(); i++) {
		CargoTruckRoute route = this->routes[i];
		ImGui::PushID((int)route.id);

		char buf[200];
		sprintf_s(buf, "Route %zu", route.id);
		if (ImGui::CollapsingHeader(buf)) {
			// Delete button
			if (ImGui::Button("Delete")) {
				this->deleteRoute(route.id);
			}
		}

		ImGui::PopID();
	}
	// Add Route button
	if (ImGui::Button("Create new route")) {
		this->addRoute(CargoTruckRoute(0));
	}

	ImGui::End();
	ImGui::PopID();
}
void TruckDepotController::onHourChange(size_t newHour) {

}

void TruckDepotController::addRoute(CargoTruckRoute r) {
	this->routes.push_back(r);
}
void TruckDepotController::deleteRoute(size_t id) {
	for (size_t i = 0; i < this->routes.size(); i++) {
		if (this->routes[i].id == id) {
			this->routes.erase(this->routes.begin() + i);
			return;
		}
	}
}
