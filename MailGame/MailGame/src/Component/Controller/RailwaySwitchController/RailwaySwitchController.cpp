#include "RailwaySwitchController.h"
#include <imgui.h>
#include "Entity/Entity.h"
#include "Game/Game.h"
#include "GameMap/GameMap.h"

RailwaySwitchController::RailwaySwitchController(sf::Vector2i t) : tile(t) {}

void RailwaySwitchController::update(float delta) {}
void RailwaySwitchController::renderUi() {
	GameMap* gMap = this->getEntity()->getGame()->getGameMap();
	size_t x = (size_t)this->tile.x;
	size_t y = (size_t)this->tile.y;
	Tile t = gMap->getTileAt(x, y);
	ImGui::Begin(this->getEntity()->getName().c_str());

	for (auto kv : t.railway.value()) {
		ImGui::PushID((int)kv.first);
		hour_t hour = UiHandler::chooseHour("Hour", kv.first);
		IsoRotation from = UiHandler::chooseDirection("From", kv.second.from);
		IsoRotation to = UiHandler::chooseDirection("To", kv.second.to);
		if (ImGui::Button("Delete")) {
			gMap->removeRailTrack(x, y, kv.first);
		} else
		if (hour != kv.first) {
			// Remove old railway
			gMap->removeRailTrack(x, y, kv.first);
			// Add new railway
			gMap->addRailTrack(x, y, from, to, hour);
		} else
		if (from != kv.second.from || to != kv.second.to) {
			this->getEntity()->getGame()->getGameMap()->addRailTrack((size_t)tile.x, (size_t)tile.y, from, to, hour);
		}
		ImGui::PopID();
	}
	if (ImGui::Button("Add railway")) {
		for (hour_t hour = 0; hour < 24; hour++) {
			if (t.railway.value().find(hour) == t.railway.value().end()) {
				gMap->addRailTrack(x, y, IsoRotation::NORTH, IsoRotation::SOUTH, hour);
				break;
			}
		}
	}
	ImGui::End();
}