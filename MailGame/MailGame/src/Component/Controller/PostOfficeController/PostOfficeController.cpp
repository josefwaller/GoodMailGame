#include "PostOfficeController.h"
#include "Entity/Entity.h"
#include "Component/Transform/Transform.h"
#include "Component/ComponentType/ComponentType.h"
#include "Component/TransitStop/TransitStop.h"
#include "Component/Controller/VehicleController/MailTruckController/MailTruckController.h"
#include "Game/Game.h"
#include "Ui/UiHandler/UiHandler.h"
#include "Entity/EntityPresets/VehiclePresets/VehiclePresets.h"
#include "Component/MailContainer/MailContainer.h"
#include "Component/Pathfinder/RoadPathfinder/RoadPathfinder.h"
#include "Mail/Mail.h"
#include "System/SaveData/SaveData.h"
#include <string>
#include <imgui.h>

void PostOfficeController::update(float delta) {
	if (this->getEntity()->transform->isOnScreen()) {
		ImGui::PushID((int)this->getEntity()->getId());
		ImGui::Begin(std::string("Post Office " + std::to_string(this->getEntity()->getId())).c_str());
		// Print num letters
		char buf[200];
		sprintf_s(buf, "%zu mail objects", this->getEntity()->mailContainer->getNumLetters());
		ImGui::Text(buf);
		for (auto it = this->routes.begin(); it != this->routes.end(); it++) {
			// Get the route index
			size_t index = (size_t)(it - this->routes.begin());
			// Get the ID as a string, for labels
			ImGui::PushID((int)it->id);
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
				if (ImGui::BeginCombo("Type", it->isDelivering ? "Delivering" : "Picking Up")) {
					if (ImGui::Selectable("Delivering")) {
						this->setRouteType(index, true);
					}
					if (ImGui::Selectable("Picking Up")) {
						this->setRouteType(index, false);
					}
					ImGui::EndCombo();
				}
				// Add the stops
				for (size_t i = 0; i < it->stops.size(); i++) {
					if (ImGui::CollapsingHeader(std::string("Stop " + std::to_string(i)).c_str())) {
						ImGui::PushID((int)i);
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
			this->addRoute(MailTruckRoute(true, 0));
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
void PostOfficeController::onHourChange(hour_t newHour) {
	// Check if any of the routes depart
	for (auto it = this->routes.begin(); it != this->routes.end(); it++) {
		if (it->departTime == newHour) {
			// Spawn a new truck for that route
			Game* game = this->getEntity()->getGame();
			auto trans = this->getEntity()->transform;
			auto transit = this->getEntity()->transitStop;
			auto truck = VehiclePresets::mailTruck(
				game,
				// For right now, just spawn the truck on the tile
				sf::Vector3f(transit->getCarStop().tile),
				IsoRotation::NORTH,
				*it,
				this->getEntity()
			);
			game->addEntity(truck);
		}
	}
}

void PostOfficeController::setStopTile(size_t routeIndex, size_t stopIndex, sf::Vector2i pos) {
	// Make sure the tile is a road
	if (this->getEntity()->getGame()->getGameMap()->hasRoadAt(pos.x, pos.y))
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
void PostOfficeController::addRoute(MailTruckRoute route) {
	this->routes.push_back(route);
}
void PostOfficeController::deleteRoute(size_t routeIndex) {
	routesToDelete.push_back(routeIndex);
}

std::optional<SaveData> PostOfficeController::getSaveData() {
	SaveData sd(componentTypeToStr(ComponentType::Controller));
	for (MailTruckRoute route : this->routes) {
		sd.addData(mailTruckRouteToSaveData(route));
	}
	return sd;
}
void PostOfficeController::fromSaveData(SaveData data) {
	for (SaveData d : data.getDatas()) {
		this->routes.push_back(saveDataToMailTruckRoute(d));
	}
}
