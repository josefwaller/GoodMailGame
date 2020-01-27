#include "PostOfficeController.h"
#include "Entity/Entity.h"
#include "Component/Transform/Transform.h"
#include "Game/Game.h"
#include "Ui/UiHandler/UiHandler.h"
#include "Entity/EntityPresets/EntityPresets.h"
#include <string>
#include <imgui.h>

void PostOfficeController::update(float delta) {
	if (this->getEntity()->transform->isOnScreen()) {
		ImGui::PushID(this->getEntity()->getId());
		ImGui::Begin(std::string("Post Office " + std::to_string(this->getEntity()->getId())).c_str());
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
				// Add the stops
				for (size_t i = 0; i < it->stops.size(); i++) {
					if (ImGui::CollapsingHeader(std::string("Stop " + std::to_string(i)).c_str())) {
						ImGui::PushID(i);
						// Add select target button
						std::string btnText = "Select Target";
						if (it->stops[i].target) {
							sf::Vector2i target = it->stops[i].target.value();
							btnText = "Target at (" + std::to_string(target.x) + ", " + std::to_string(target.y) + ")";
						}
						if (ImGui::Button(btnText.c_str())) {
							this->getEntity()->getGame()->getUi()->selectTile([this, index, i](sf::Vector2i pos) {
								this->setStopTile(index, i, pos);
							});
						}
						// Add Delete stop button
						if (ImGui::Button("Delete Stop")) {
							this->deleteStop(index, i);
						}
						ImGui::PopID();
					}
				}
				// Add stop button
				if (ImGui::Button("Add New Stop")) {
					this->addStop(index, MailTruckRouteStop());
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
void PostOfficeController::onHourChange(size_t newHour) {
	// Todo: spawn a truck if there is a route departing at this hour
	Game* game = this->getEntity()->getGame();
	game->addEntity(EntityPresets::mailTruck(game, this->getEntity()->transform->getPosition(), IsoRotation::NORTH));
}

void PostOfficeController::setStopTile(size_t routeIndex, size_t stopIndex, sf::Vector2i pos) {
	// Todo: Make sure this is safe?
	this->routes[routeIndex].stops[stopIndex].target = pos;
}
void PostOfficeController::addStop(size_t routeIndex, MailTruckRouteStop stop) {
	this->routes[routeIndex].stops.push_back(stop);
}
void PostOfficeController::deleteStop(size_t routeIndex, size_t stopIndex) {
	std::vector<MailTruckRouteStop>* stops = &this->routes[routeIndex].stops;
	stops->erase(stops->begin() + stopIndex);
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
