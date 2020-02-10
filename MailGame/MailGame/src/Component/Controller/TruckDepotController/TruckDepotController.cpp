#include "TruckDepotController.h"
#include "PostalCodeDatabase/PostalCodeDatabase.h"
#include "Component/Transform/Transform.h"
#include "Entity/EntityPresets/EntityPresets.h"
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

			// Departure time
			sprintf_s(buf, "%d:00", route.departureTime);
			if (ImGui::BeginCombo("Departure Time", buf)) {
				
				for (int hr = 0; hr < 24; hr++) {
					sprintf_s(buf, "%d:00", hr);
					if (ImGui::Selectable(buf, hr == route.departureTime)) {
						this->setRouteDepartTime(route.id, hr);
					}
				}
				ImGui::EndCombo();
			}

			// GUI for the stops
			for (size_t j = 0; j < route.stops.size(); j++) {
				CargoTruckStop stop = route.stops[j];
				ImGui::PushID((int)stop.id);
				
				sprintf_s(buf, "Stop %zd", stop.id);
				if (ImGui::CollapsingHeader(buf)) {
					// Target button
					if (stop.target.lock()) {
						sprintf_s(buf, "Target with id = %zu", stop.target.lock()->getId());
					}
					else {
						sprintf_s(buf, "No Target");
					}

					if (ImGui::Button(buf)) {
						this->getEntity()->getGame()->getUi()->selectEntity(
							[this, route, j](std::weak_ptr<Entity> e) {
								this->setStopTarget(j, route.id, e);
						});
					}
					// Pick up dropdown
					if (ImGui::CollapsingHeader("Pick up")) {
						for (long long c : stop.toPickUp) {
							sprintf_s(buf, "%lu", c);
							if (ImGui::Button(buf)) {
								this->removeStopPickUp(j, route.id, c);
							}
						}
						if (ImGui::BeginCombo("Pick Up", "0")) {
							for (long long c : PostalCodeDatabase::get()->getAllIds()) {
								sprintf_s(buf, "%lu", c);
								if (ImGui::Selectable(buf)) {
									this->setStopPickUp(j, route.id, c);
								}
							}
							ImGui::EndCombo();
						}
					}
					// Drop off dropdown
					if (ImGui::CollapsingHeader("Drop off")) {
						for (long long c : stop.toDropOff) {
							sprintf_s(buf, "%lu", c);
							if (ImGui::Button(buf)) {
								this->removeStopDropOff(j, route.id, c);
							}
						}
						if (ImGui::BeginCombo("Drop Off", "0")) {
							for (long long c : PostalCodeDatabase::get()->getAllIds()) {
								sprintf_s(buf, "%lu", c);
								if (ImGui::Selectable(buf)) {
									this->setStopDropOff(j, route.id, c);
								}
							}
							ImGui::EndCombo();
						}
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
	for (auto it: this->routes) {
		if (it.second.departureTime == newHour) {
			this->getEntity()->getGame()->addEntity(
				EntityPresets::cargoTruck(
					this->getEntity()->getGame(),
					this->getEntity()->transform->getPosition()
					+ this->getEntity()->transform->getRotation().getUnitVector(),
					IsoRotation::NORTH,
					it.second,
					this->getEntity()
				)
			);
		}
	}
}

void TruckDepotController::addRoute(CargoTruckRoute r) {
	this->routes.insert({ r.id, r });
}
void TruckDepotController::deleteRoute(size_t id) {
	this->toDelete.push_back(this->routes.find(id)->second);
}
void TruckDepotController::setRouteDepartTime(size_t routeId, int depTime) {
	this->routes.find(routeId)->second.departureTime = depTime;
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
void TruckDepotController::setStopPickUp(size_t stopIndex, size_t routeId, long long code) {
	this->routes.find(routeId)->second.stops[stopIndex].toPickUp.push_back(code);
}
void TruckDepotController::removeStopPickUp(size_t stopIndex, size_t routeId, long long code) {
	std::vector<long long>* toPickUp = &this->routes.find(routeId)->second.stops[stopIndex].toPickUp;
	toPickUp->erase(std::remove(toPickUp->begin(), toPickUp->end(), code), toPickUp->end());
}
void TruckDepotController::setStopDropOff(size_t stopIndex, size_t routeId, long long code) {
	this->routes.find(routeId)->second.stops[stopIndex].toDropOff.push_back(code);
}
void TruckDepotController::removeStopDropOff(size_t stopIndex, size_t routeId, long long code) {
	std::vector<long long>* toDropOff = &this->routes.find(routeId)->second.stops[stopIndex].toDropOff;
	toDropOff->erase(std::remove(toDropOff->begin(), toDropOff->end(), code), toDropOff->end());
}
