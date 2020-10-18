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
#include "System/Utils/Utils.h"
#include "System/SaveData/SaveData.h"
#include "VehicleModel/VehicleModel.h"
#include <string>
#include <imgui.h>

void PostOfficeController::update(float delta) {
	if (this->getEntity()->transform->isOnScreen()) {
		ImGui::PushID((int)this->getEntity()->getId());
		sf::Vector3f pos = this->getEntity()->transform->getPosition();
		// Print num letters
		char buf[200];
		sprintf_s(buf, "Post Office at (%f, %f)", pos.x, pos.y);
		ImGui::Begin(buf);
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
				prepareRouteForTruck(*it),
				this->getEntity()
			);
			game->addEntity(truck);
		}
	}
}

MailTruckRoute PostOfficeController::prepareRouteForTruck(MailTruckRoute route) {
	gtime_t mTime = this->getEntity()->getGame()->getMidnightTime();
	for (auto stop = route.stops.begin(); stop != route.stops.end(); stop++) {
		for (auto p = stop->points.begin(); p != stop->points.end(); p++) {
			p->expectedTime += mTime;
		}
	}
	return route;
}

void PostOfficeController::setStopTile(size_t routeIndex, size_t stopIndex, sf::Vector2i pos) {
	// Make sure the tile is a road
	if (this->getEntity()->getGame()->getGameMap()->hasRoadAt(pos.x, pos.y))
		this->routes[routeIndex].stops[stopIndex].target = pos;
	resetRoutePoints();
}
void PostOfficeController::addStop(size_t routeIndex, MailTruckRouteStop stop) {
	this->routes[routeIndex].stops.push_back(stop);
	resetRoutePoints();
}
void PostOfficeController::deleteStop(size_t routeIndex, size_t stopIndex) {
	std::vector<MailTruckRouteStop>* stops = &this->routes[routeIndex].stops;
	stops->erase(stops->begin() + stopIndex);
	resetRoutePoints();
}
void PostOfficeController::setRouteTime(size_t routeIndex, int time) {
	this->routes[routeIndex].departTime = time;
	resetRoutePoints();
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
std::vector<RoutePoint> PostOfficeController::toRoutePointVector(std::vector<sf::Vector3f> points, gtime_t time) {
	std::vector<RoutePoint> toReturn;
	sf::Vector3f lastPos = points.front();
	for (sf::Vector3f p : points) {
		time + Utils::getVectorDistance(lastPos, p);
		toReturn.push_back(RoutePoint(p, time));
	}
	return toReturn;
}
void PostOfficeController::resetRoutePoints() {
	const float SPEED = VehicleModelInfo::getModelInfo(VehicleModel::MailTruck).getSpeed();
	for (auto r = this->routes.begin(); r != this->routes.end(); r++) {
		MailTruckRoute route = *r;
		gtime_t time = route.departTime * Game::UNITS_IN_GAME_HOUR;
		for (auto s = r->stops.begin(); s != r->stops.end(); s++) {
			if (!s->target.has_value())
				continue;
			sf::Vector2i v = s->target.value();
			if (s == r->stops.begin()) {
				std::vector<RoutePoint> path = this->toRoutePointVector(
					this->getEntity()->transitStop->getDepartingTransitPath(this->getEntity(), TransitStop::TransitType::Car),
					time
				);
				time = path.back().expectedTime;
				std::vector<RoutePoint> newPath = this->getEntity()->pathfinder->findPathBetweenPoints(
					path.back().pos,
					sf::Vector3f(v.x, v.y, 0.0f),
					time,
					SPEED * 0.75
				);
				path.insert(path.end(), newPath.begin(), newPath.end());
				s->points = path;
			}
			else {
				sf::Vector2i prevStop = (s - 1)->target.value();
				s->points = this->getEntity()->pathfinder->findPathBetweenPoints(
					sf::Vector3f(prevStop.x, prevStop.y, 0.0f),
					sf::Vector3f(v.x, v.y, 0.0f),
					time,
					SPEED
				);
			}
			time = s->points.back().expectedTime;
		}
	}
}
