#include "./BoatController.h"
#include "Entity/Entity.h"
#include "GameMap/GameMap.h"
#include "Component/Ai/Ai.h"
#include "System/Utils/Utils.h"
#include "Component/Pathfinder/Pathfinder.h"
#include "Component/Transform/Transform.h"
#include "Component/ComponentType/ComponentType.h"

BoatController::BoatController(gtime_t departTime, VehicleModel model, std::vector<std::weak_ptr<Entity>> cargoCars) : VehicleController(departTime, model, cargoCars), stopIndex(0) {}
void BoatController::init() {
	this->stops = this->getEntity()->ai->getStops();
	this->stopIndex = 0;
	this->onArriveAtDest(this->departTime);
}

void BoatController::onArriveAtDest(gtime_t arriveTime) {
	this->stopIndex++;
	if (this->stopIndex >= this->stops.size()) {
		this->getEntity()->ai->onArriveAtDest();
	}
	else {
		this->getEntity()->ai->onArriveAtStop(this->stopIndex - 1);
		sf::Vector2i start = Utils::toVector2i(this->points.empty() ? this->getEntity()->transform->getPosition() : this->points.back().pos);
		sf::Vector2i end;
		auto e = this->stops.at(this->stopIndex).getEntityTarget().lock();
		if (e->tag == EntityTag::Warehouse) {
			auto docks = getConnectedDocks(e, EntityTag::BoatDock);
			end = docks.front();
		}
		else {
			end = Utils::toVector2i(e->transform->getPosition());
		}
		auto path = Pathfinder::findBoatPath(this->getEntity()->getGameMap(), start, end);
		std::vector<SpeedPoint> s;
		for (sf::Vector2i p : path) {
			s.push_back(SpeedPoint(sf::Vector3f(p.x, p.y, GameMap::SEA_LEVEL)));
		}
		s.front().setSpeed(0.0f);
		s.back().setSpeed(0.0f);
		this->setPoints(Utils::speedPointVectorToRoutePointVector(s, arriveTime, this->model, 0.0f));
	}
}

std::optional<SaveData> BoatController::getSaveData() {
	using namespace SaveKeys;
	SaveData data(componentTypeToStr(ComponentType::Controller));
	data.addVehicleControllerStopVector(STOPS, this->stops);
	data.addSizeT(STOP_INDEX, this->stopIndex);
	data.addData(VehicleController::getSaveData().value());
	return data;
}
void BoatController::fromSaveData(SaveData data) {
	using namespace SaveKeys;
	VehicleController::fromSaveData(data.getData(VEHICLE_CONTROLLER));
	this->stops = data.getVehicleControllerStopVector(STOPS, this->getEntity()->getGame());
	this->stopIndex = data.getSizeT(STOP_INDEX);
}