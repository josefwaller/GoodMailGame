#include "PostOfficeController.h"
#include "Entity/Entity.h"
#include "Component/Transform/Transform.h"
#include "Component/ComponentType/ComponentType.h"
#include "Component/TransitStop/TransitStop.h"
#include "Game/Game.h"
#include "Ui/UiHandler/UiHandler.h"
#include "Entity/EntityPresets/VehiclePresets/VehiclePresets.h"
#include "Component/MailContainer/MailContainer.h"
#include "Component/Pathfinder/RoadPathfinder/RoadPathfinder.h"
#include "Mail/Mail.h"
#include "System/Utils/Utils.h"
#include "System/SaveData/SaveData.h"
#include "VehicleModel/VehicleModel.h"
#include "PostalCodeDatabase/PostalCodeDatabase.h"
#include <string>
#include <imgui.h>

void PostOfficeController::renderUi() {
	ImGui::PushID((int)this->getEntity()->getId());
	sf::Vector3f pos = this->getEntity()->transform->getPosition();
	// Print num letters
	char buf[200];
	sprintf_s(buf, "Post Office at (%f, %f)", pos.x, pos.y);
	ImGui::Begin(buf);
	sprintf_s(buf, "%zu mail objects", this->getEntity()->mailContainer->getNumLetters());
	ImGui::Text(buf);
	auto mInfo = VehicleModelInfo::getModelInfo(VehicleModel::MailTruck);
	for (auto it = this->routes.begin(); it != this->routes.end(); it++) {
		// Get the route index
		size_t index = (size_t)(it - this->routes.begin());
		// Get the ID as a string, for labels
		ImGui::PushID((int)it->id);
		// Create a collapsing header
		std::string routeName = "Route " + std::to_string(it->id);
		if (ImGui::CollapsingHeader(routeName.c_str())) {
			// Show cost
			sprintf_s(buf, "Cost: $%d.00 ($%d.00 + %fm * $%d/m)", this->getRouteCost(*it), mInfo.getInitialCost(), it->length, mInfo.getCostPerTile());
			ImGui::Text(buf);
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

	// Choose color of postal code
	if (this->postalCodeId.has_value()) {
		PostalCode pCode = PostalCodeDatabase::get()->getPostalCode(this->postalCodeId.value());
		sf::Color pColor = pCode.getColor();
		float color[3] = { pColor.r / 255.0f, pColor.g / 255.0f, pColor.b / 255.0f };
		ImGui::ColorEdit3("Postal code color", color);
		pColor = sf::Color(color[0] * 255.0f, color[1] * 255.0f, color[2] * 255.0f, 100);
		pCode.setColor(pColor);
		PostalCodeDatabase::get()->setPostalCode(this->postalCodeId.value(), pCode);
	}
	ImGui::End();
	ImGui::PopID();
}

void PostOfficeController::update(float) {
	// Delete any routes that are flagged for deletion
	if (!this->routesToDelete.empty()) {
		for (int i = (int)(routes.size() - 1); i >= 0; i--) {
			// Check if the route should be deleted
			if (std::find(this->routesToDelete.begin(), this->routesToDelete.end(), i) != this->routesToDelete.end()) {
				// Delete it
				this->routes.erase(this->routes.begin() + i);
			}
		}
		this->routesToDelete.clear();
		this->resetPostalCode();
	}
}
void PostOfficeController::onHourChange(hour_t newHour) {
	// Check if any of the routes depart
	for (auto it = this->routes.begin(); it != this->routes.end(); it++) {
		if (it->departTime == newHour && it->stops.size() >= 1) {
			gtime_t spawnTime = this->getEntity()->getGame()->getMidnightTime() + Game::UNITS_IN_GAME_HOUR * it->departTime;
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
				this->getEntity(),
				spawnTime
			);
			game->addEntity(truck);
		}
	}
}

money_t PostOfficeController::getRouteCost(MailTruckRoute r) {
	auto mInfo = VehicleModelInfo::getModelInfo(VehicleModel::MailTruck);
	return mInfo.getInitialCost() + r.length * mInfo.getCostPerTile();
}
money_t PostOfficeController::getCost() {
	money_t toReturn = 0;
	for (auto it = this->routes.begin(); it != this->routes.end(); it++) {
		toReturn += this->getRouteCost(*it);
	}
	return toReturn;
}

MailTruckRoute PostOfficeController::prepareRouteForTruck(MailTruckRoute route) {
	return route;
}

void PostOfficeController::setStopTile(size_t routeIndex, size_t stopIndex, sf::Vector2i pos) {
	// Make sure the tile is a road
	if (this->getEntity()->getGame()->getGameMap()->hasRoadAt(pos.x, pos.y)) {
		this->routes[routeIndex].stops[stopIndex].target = pos;
		if (this->routes.at(routeIndex).isDelivering) {
			this->resetPostalCode();
		}
		this->routes[routeIndex].length = this->getRouteLength(this->routes[routeIndex]);
	}
}
void PostOfficeController::addStop(size_t routeIndex, MailTruckRouteStop stop) {
	this->routes[routeIndex].stops.push_back(stop);
	// Maybe will need to reset postal codes here but for now stop is always empty
}
void PostOfficeController::deleteStop(size_t routeIndex, size_t stopIndex) {
	std::vector<MailTruckRouteStop>* stops = &this->routes[routeIndex].stops;
	stops->erase(stops->begin() + stopIndex);
	if (this->routes.at(routeIndex).isDelivering) {
		this->resetPostalCode();
	}
	this->routes[routeIndex].length = this->getRouteLength(this->routes[routeIndex]);
}
void PostOfficeController::setRouteTime(size_t routeIndex, int time) {
	this->routes[routeIndex].departTime = time;
}
void PostOfficeController::setRouteType(size_t routeIndex, bool isDelivering) {
	this->routes[routeIndex].isDelivering = isDelivering;
	this->resetPostalCode();
}
void PostOfficeController::addRoute(MailTruckRoute route) {
	this->routes.push_back(route);
	// Maybe will need to reset postal codes here, but for now the route is always empty
}
void PostOfficeController::deleteRoute(size_t routeIndex) {
	routesToDelete.push_back(routeIndex);
}

std::optional<SaveData> PostOfficeController::getSaveData() {
	SaveData sd(componentTypeToStr(ComponentType::Controller));
	for (MailTruckRoute route : this->routes) {
		sd.addData(route.getSaveData());
	}
	return sd;
}
void PostOfficeController::fromSaveData(SaveData data) {
	for (SaveData d : data.getDatas()) {
		this->routes.push_back(MailTruckRoute(d));
	}
	for (auto it = this->routes.begin(); it != this->routes.end(); it++) {
		it->length = this->getRouteLength(*it);
	}
	this->resetPostalCode();
}

void PostOfficeController::resetPostalCode() {
	// Create postal code if it does not exist
	if (!this->postalCodeId.has_value()) {
		this->postalCodeId = PostalCodeDatabase::get()->createPostalCode(
			"Postal code for " + this->getEntity()->getName()
		);
	}
	// Gather all the routes that are delivering
	std::vector<MailTruckRoute> delivering;
	for (auto it = this->routes.begin(); it != this->routes.end(); it++) {
		if (it->isDelivering) {
			delivering.push_back(*it);
		}
	}
	// Get all the paths the routes take
	std::vector<std::vector<SpeedPoint>> paths;
	for (MailTruckRoute r : delivering) {
		std::vector<SpeedPoint> path = { SpeedPoint(this->getEntity()->transform->getPosition()) };
		sf::Vector3f dockPoint = path.back().getPos();
		for (MailTruckRouteStop target : r.stops) {
			if (target.target.has_value()) {
				// Time and speed shouldn't matter here
				std::vector<SpeedPoint> p = this->getEntity()->pathfinder->findPathBetweenPoints(path.back().getPos(), Utils::toVector3f(target.target.value()), 0, 1.0f);
				path.insert(path.end(), p.begin(), p.end());
			}
		}
		// Finally it comes pack to the office
		std::vector<SpeedPoint> p = this->getEntity()->pathfinder->findPathBetweenPoints(path.back().getPos(), dockPoint, 0, 1.0f);
		path.insert(path.end(), p.begin(), p.end());
		paths.push_back(path);
	}
	// Now set all those to the post code
	// first reset all the tiles
	for (size_t x = 0; x < GameMap::MAP_WIDTH; x++) {
		for (size_t y = 0; y < GameMap::MAP_HEIGHT; y++) {
			if (this->getEntity()->getGameMap()->getTileAt(x, y).postalCode == this->postalCodeId.value()) {
				this->getEntity()->getGameMap()->setCodeForTile(x, y, PostalCodeDatabase::get()->getDefaultPostalCode());
			}
		}
	}
	// Now set all the tiles to our code
	for (std::vector<SpeedPoint> path : paths) {
		for (SpeedPoint point : path) {
			for (sf::Vector3f offset : {sf::Vector3f(0, 0, 0), sf::Vector3f(1, 0, 0), sf::Vector3f(-1, 0, 0), sf::Vector3f(0, 1, 0), sf::Vector3f(0, -1, 0)}) {
				sf::Vector3f toSet = point.getPos() + offset;
				this->getEntity()->getGameMap()->setCodeForTile((size_t)floor(toSet.x), (size_t)floor(toSet.y), this->postalCodeId.value());
			}
		}
	}
}

size_t PostOfficeController::getRouteLength(MailTruckRoute r) {
	using Segment = Pathfinder::RoadSegment;
	size_t len = 0;
	sf::Vector2i dockPoint = Utils::toVector2i(this->getEntity()->transform->getPosition());
	sf::Vector2i lastPoint = dockPoint;
	std::vector<Segment> path = { Segment(lastPoint) };
	for (MailTruckRouteStop target : r.stops) {
		if (target.target.has_value()) {
			// Time and speed shouldn't matter here
			std::vector<Segment> p = Pathfinder::findCarPath(this->getEntity()->getGameMap(), lastPoint, target.target.value());
			path.insert(path.end(), p.begin(), p.end());
			// Set last point
			auto var = p.back();
			if (var.getType() == Segment::Type::Tile) {
				lastPoint = var.getTile();
			}
			else {
				lastPoint = Utils::toVector2i(var.getTunnel().getEntrances().first.getPosition());
			}
		}
	}
	// Finally it comes pack to the office
	std::vector<Segment> p = Pathfinder::findCarPath(this->getEntity()->getGameMap(), lastPoint, dockPoint);
	path.insert(path.end(), p.begin(), p.end());
	// Now we just sum up the distance
	sf::Vector2f prev = sf::Vector2f(path.front().getTile());
	float length = 0.0f;
	for (auto it = path.begin(); it != path.end(); it++) {
		if (it->getType() == Segment::Type::Tile) {
			length += Utils::getVectorDistance(sf::Vector2f(it->getTile()), prev);
			prev = sf::Vector2f(it->getTile());
		}
		else if (it->getType() == Segment::Type::Tunnel) {
			// Since we know that the pathfinding will go directly to the entrance and exit of the tunnel, we just need to add the tunnel length
			length += it->getTunnel().getLength();
			if (it->getTunnelReversed()) {
				prev = sf::Vector2f(Utils::toVector2i(it->getTunnel().getEntrances().first.getPosition()));
			}
			else {
				prev = sf::Vector2f(Utils::toVector2i(it->getTunnel().getEntrances().second.getPosition()));
			}
		}
	}
	return length;
}