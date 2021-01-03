#include "DepotController.h"
#include "PostalCodeDatabase/PostalCodeDatabase.h"
#include "Component/Transform/Transform.h"
#include "Component/ComponentType/ComponentType.h"
#include "Component/Pathfinder/Pathfinder.h"
#include "System/Utils/Utils.h"
#include <imgui.h>
#include "Entity/Entity.h"
#include "Game/Game.h"
#include "System/SaveData/SaveData.h"
#include "TechTree/TechTree.h"

DepotController::DepotController(TransitType t, std::vector<VehicleModel> models) : type(t), availableModels(models) {}

void DepotController::renderUi() {
	ImGui::PushID((int)this->getEntity()->getId());
	char buf[200];
	sf::Vector3f pos = this->getEntity()->transform->getPosition();
	std::string s = entityTagToString(this->getEntity()->tag);
	sprintf_s(buf, "%s at (%f, %f)", s.c_str(), pos.x, pos.y);
	ImGui::Begin(buf);
	if (ImGui::Button(this->showRoutes ? "Hide Routes" : "Show Routes")) {
		this->showRoutes = !this->showRoutes;
	}

	// Show gui for each route
	for (auto kvp : this->routes) {
		TransitRoute route = kvp.second;
		ImGui::PushID((int)route.id);

		sprintf_s(buf, "Route %zu", route.id);
		if (ImGui::CollapsingHeader(buf)) {
			// Departure time
			sprintf_s(buf, "%d:00", route.departTime);
			if (ImGui::BeginCombo("Departure Time", buf)) {
				for (int hr = 0; hr < 24; hr++) {
					sprintf_s(buf, "%d:00", hr);
					if (ImGui::Selectable(buf, hr == route.departTime)) {
						this->setRouteDepartTime(route.id, hr);
					}
				}
				ImGui::EndCombo();
			}
			VehicleModelInfo modelInfo = VehicleModelInfo::getModelInfo(route.model);
			if (ImGui::BeginCombo("Vehicle Model", modelInfo.getName().c_str())) {
				for (auto it = this->availableModels.begin(); it != this->availableModels.end(); it++) {
					VehicleModelInfo mInfo = VehicleModelInfo::getModelInfo(*it);
					if (TechTree::getTechUnlocked(mInfo.getRequiredTechnology())) {
						if (ImGui::Selectable(mInfo.getName().c_str(), *it == route.model)) {
							this->setRouteModel(route.id, *it);
						}
					}
				}
				ImGui::EndCombo();
			}
			if (modelInfo.getMaxNumCargoCars() != 0) {
				sprintf_s(buf, "%s", route.cargoCarModel.has_value() ? CargoCarInfo::get(route.cargoCarModel.value()).getName().c_str() : "No cargo cars");
				if (ImGui::BeginCombo("Cargo Car", buf)) {
					const auto allowed = modelInfo.getAllowedCargoCars();
					for (auto it = allowed.begin(); it != allowed.end(); it++) {
						CargoCarInfo cInfo = CargoCarInfo::get(*it);

						if (ImGui::Selectable(cInfo.getName().c_str(), route.cargoCarModel.has_value() && *it == route.cargoCarModel.value())) {
							this->setRouteCargoCarModel(route.id, { *it });
						}
					}
					if (ImGui::Selectable("No cargo cars", !route.cargoCarModel.has_value())) {
						this->setRouteCargoCarModel(route.id, {});
					}
					ImGui::EndCombo();
				}
				if (route.cargoCarModel.has_value()) {
					int x = route.numCargoCars;

					ImGui::SliderInt("Num Cars", &x, 0, modelInfo.getMaxNumCargoCars());
					if (x != route.numCargoCars) {
						this->setRouteNumberCargoCars(route.id, (unsigned int)x);
					}
				}
			}

			// GUI for the stops
			for (size_t j = 0; j < route.stops.size(); j++) {
				TransitRouteStop stop = route.stops[j];
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
							sprintf_s(buf, "%llu", c);
							if (ImGui::Button(buf)) {
								this->removeStopPickUp(j, route.id, c);
							}
						}
						if (ImGui::BeginCombo("Pick Up", "0")) {
							for (long long c : PostalCodeDatabase::get()->getAllIds()) {
								sprintf_s(buf, "%llu", c);
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
							sprintf_s(buf, "%llu", c);
							if (ImGui::Button(buf)) {
								this->removeStopDropOff(j, route.id, c);
							}
						}
						if (ImGui::BeginCombo("Drop Off", "0")) {
							for (long long c : PostalCodeDatabase::get()->getAllIds()) {
								sprintf_s(buf, "%llu", c);
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
				this->addStop(TransitRouteStop(), route.id);
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
		if (this->availableModels.empty())
			throw std::runtime_error("No available models in DepotController!");
		this->addRoute(TransitRoute(0, this->availableModels.front()));
	}

	ImGui::End();
	ImGui::PopID();
}
void DepotController::update(float) {
	// Delete routes
	for (TransitRoute r : this->toDelete) {
		this->routes.erase(r.id);
	}
	this->toDelete.clear();

	if (this->showRoutes) {
		for (auto kv : this->routes) {
			std::vector<RoutePoint> routePoints;
			TransitRoute route = kv.second;
			gtime_t time = route.departTime * Game::UNITS_IN_GAME_HOUR;
			VehicleModelInfo modelInfo = VehicleModelInfo::getModelInfo(route.model);
			if (kv.second.stops.empty()) {
				continue;
			}
			// Add the depot as another stop
			TransitRouteStop stop;
			stop.target = this->getEntity();
			route.stops.push_back(stop);
			// Add paths between the stops
			for (auto stop = route.stops.begin(); stop != route.stops.end(); stop++) {
				if (!stop->target.lock())
					continue;
				// Get the departing building
				std::weak_ptr<Entity> departBuilding;
				if (stop == route.stops.begin()) {
					departBuilding = this->getEntity();
				}
				else {
					// Departs from the previous stop's building
					departBuilding = (stop - 1)->target.lock();
				}
				// Get the departing path
				std::vector<RoutePoint> departPath = Utils::speedPointVectorToRoutePointVector(
					this->getEntity()->transitStop->getDepartingTransitPath(departBuilding.lock(), this->type),
					time,
					modelInfo.getSpeed()
				);
				if (stop != route.stops.end() - 1) {
					time = departPath.back().expectedTime;
					routePoints.insert(routePoints.end(), departPath.begin(), departPath.end());
				}
				// Get the arriving path without time cause we don't know what time we'll be there yet
				std::vector<SpeedPoint> arriving = stop->target.lock()->transitStop->getArrivingTransitPath(
					stop->target.lock(),
					this->type
				);
				// Get the path between
				// TODO: Fix speed
				time = departPath.back().expectedTime;
				std::vector<RoutePoint> path = this->getEntity()->pathfinder->findPathBetweenPoints(
					departPath.back().pos,
					arriving.front().getPos(),
					time,
					modelInfo.getSpeed()
				);
				if (!path.empty()) {
					routePoints.insert(routePoints.end(), path.begin(), path.end());
					time = path.back().expectedTime;
				}
				// Get arriving path
				std::vector<RoutePoint> arrivalPath = Utils::speedPointVectorToRoutePointVector(
					arriving,
					time,
					modelInfo.getSpeed()
				);
				routePoints.insert(routePoints.end(), arrivalPath.begin(), arrivalPath.end());
				time = arrivalPath.back().expectedTime;
			}
			this->getEntity()->getGame()->getUi()->addPathToDraw(routePoints);
		}
	}
}
void DepotController::onHourChange(hour_t newHour) {
	for (auto it : this->routes) {
		if (it.second.departTime == newHour) {
			this->spawnVehicleForRoute(it.second);
		}
	}
}

money_t DepotController::getCost() {
	money_t toReturn = 0;
	for (auto it = this->routes.begin(); it != this->routes.end(); it++) {
		toReturn += VehicleModelInfo::getModelInfo(it->second.model).getCostPerRoute();
	}
	return toReturn;
}

TransitRoute DepotController::prepareRouteForVehicle(TransitRoute route) {
	return route;
}

void DepotController::addRoute(TransitRoute r) {
	this->routes.insert({ r.id, r });
}
void DepotController::deleteRoute(size_t id) {
	this->toDelete.push_back(this->routes.find(id)->second);
}
void DepotController::setRouteDepartTime(size_t routeId, int depTime) {
	this->routes.find(routeId)->second.departTime = depTime;
}
void DepotController::setRouteModel(size_t routeId, VehicleModel model) {
	this->routes.find(routeId)->second.model = model;
	// Make sure we don't go over the maximum number of cars
	VehicleModelInfo mInfo = VehicleModelInfo::getModelInfo(model);
	this->routes.find(routeId)->second.numCargoCars = std::min(mInfo.getMaxNumCargoCars(), this->routes.find(routeId)->second.numCargoCars);
}
void DepotController::setRouteCargoCarModel(size_t routeId, std::optional<CargoCarModel> model) {
	this->routes.find(routeId)->second.cargoCarModel = model;
}
void DepotController::setRouteNumberCargoCars(size_t routeId, unsigned int num) {
	this->routes.find(routeId)->second.numCargoCars = num;
}
void DepotController::addStop(TransitRouteStop stop, size_t routeId) {
	this->routes.find(routeId)->second.stops.push_back(stop);
}
void DepotController::deleteStop(size_t stopIndex, size_t routeId) {
	this->routes.find(routeId)->second.stops.erase(this->routes.find(routeId)->second.stops.begin() + stopIndex);
}
void DepotController::setStopTarget(size_t stopIndex, size_t routeId, std::weak_ptr<Entity> target) {
	this->routes.find(routeId)->second.stops[stopIndex].target = target;
}
void DepotController::setStopPickUp(size_t stopIndex, size_t routeId, long long code) {
	this->routes.find(routeId)->second.stops[stopIndex].toPickUp.push_back(code);
}
void DepotController::removeStopPickUp(size_t stopIndex, size_t routeId, long long code) {
	std::vector<long long>* toPickUp = &this->routes.find(routeId)->second.stops[stopIndex].toPickUp;
	toPickUp->erase(std::remove(toPickUp->begin(), toPickUp->end(), code), toPickUp->end());
}
void DepotController::setStopDropOff(size_t stopIndex, size_t routeId, long long code) {
	this->routes.find(routeId)->second.stops[stopIndex].toDropOff.push_back(code);
}
void DepotController::removeStopDropOff(size_t stopIndex, size_t routeId, long long code) {
	std::vector<long long>* toDropOff = &this->routes.find(routeId)->second.stops[stopIndex].toDropOff;
	toDropOff->erase(std::remove(toDropOff->begin(), toDropOff->end(), code), toDropOff->end());
}

std::optional<SaveData> DepotController::getSaveData() {
	SaveData sd(componentTypeToStr(ComponentType::Controller));
	for (auto kv : this->routes) {
		sd.addData(kv.second.getSaveData());
	}
	return sd;
}
void DepotController::fromSaveData(SaveData data) {
	for (SaveData d : data.getDatas()) {
		TransitRoute r(d, this->getEntity()->getGame());
		this->routes.insert({ r.id, r });
	}
}