#include "TruckDepotController.h"
#include <imgui.h>
#include "Entity/Entity.h"
#include "Game/Game.h"

void TruckDepotController::update(float delta) {
	ImGui::PushID((int)this->getEntity()->getId());
	ImGui::Begin("Truck Depot");
	char buf[200];

	// Show gui for each route
	for (auto kvp: this->routes) {
		CargoTruckRoute route = kvp.second;
		ImGui::PushID((int)route.id);


		sprintf_s(buf, "Route %zu", route.id);
		if (ImGui::CollapsingHeader(buf)) {

			// GUI for the stops
			for (size_t j = 0; j < route.stops.size(); j++) {
				ImGui::PushID((int)j);
				CargoTruckStop stop = route.stops[j];
				if (stop.target.lock()) {
					sprintf_s(buf, "Stop at %zu", stop.target.lock()->getId());
				}
				else {
					sprintf_s(buf, "Empty Stop %d", j);
				}
				if (ImGui::CollapsingHeader(buf)) {
					if (ImGui::Button("Target")) {
						this->getEntity()->getGame()->getUi()->selectEntity(
							[this, route, j](std::weak_ptr<Entity> e) {
								this->setStopTarget(j, route.id, e);
						});
					}
					// Delete stop button
					if (ImGui::Button("Delete Stop")) {
						this->deleteStop(j, route.id);
					}
				}

				ImGui::PopID();
			}
			// Add Stop button
			if (ImGui::Button("Add Stop")) {
				this->addStop(CargoTruckStop(), route.id);
			}
			// Delete route button
			if (ImGui::Button("Delete Route")) {
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

	// Delete routes
	for (CargoTruckRoute r : this->toDelete) {
		this->routes.erase(r.id);
	}
	this->toDelete.clear();
}
void TruckDepotController::onHourChange(size_t newHour) {

}

void TruckDepotController::addRoute(CargoTruckRoute r) {
	this->routes.insert({ r.id, r });
}
void TruckDepotController::deleteRoute(size_t id) {
	this->toDelete.push_back(this->routes.find(id)->second);
}
void TruckDepotController::addStop(CargoTruckStop stop, size_t routeId) {
	this->routes.find(routeId)->second.stops.push_back(stop);
}
void TruckDepotController::deleteStop(size_t stopIndex, size_t routeId) {
	this->routes.find(routeId)->second.stops.erase(this->routes.find(routeId)->second.stops.begin() + stopIndex);
}
void TruckDepotController::setStopTarget(size_t stopIndex, size_t routeId, std::weak_ptr<Entity> target) {
	this->routes.find(routeId)->second.stops[stopIndex].target = target;
}
