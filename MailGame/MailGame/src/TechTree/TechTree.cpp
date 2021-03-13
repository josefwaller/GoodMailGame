#include "TechTree.h"
#include <imgui.h>

std::map<Technology, TechTreeNode> TechTree::nodes = {
	{ Technology::CargoTrucks, TechTreeNode(Technology::Default, "Cargo Trucks") },
	{ Technology::FastCargoTrucks, TechTreeNode(Technology::CargoTrucks, "Faster Cargo Trucks")},
	{ Technology::Trains, TechTreeNode(Technology::CargoTrucks, "Trains") },
	{ Technology::FastTrains, TechTreeNode(Technology::Trains, "Fast Trains") },
	{ Technology::Airplanes, TechTreeNode(Technology::Trains, "Airplanes") },
	{ Technology::FastAirplanes, TechTreeNode(Technology::Airplanes, "Fast Airplanes")}
};

bool TechTree::getTechUnlocked(Technology tech) {
	// Default technology is always unlocked
	if (tech == Technology::Default)
		return true;
	return TechTree::nodes.find(tech)->second.getIsUnlocked();
}

void TechTree::unlockTech(Technology tech) {
	TechTree::nodes.find(tech)->second.unlock();
}

void TechTree::update() {
	ImGui::Begin("Tech Tree");
	for (auto kv : TechTree::nodes) {
		TechTreeNode* n = &TechTree::nodes.find(kv.first)->second;
		if (getTechUnlocked(n->getParent()) && !n->getIsUnlocked()) {
			if (ImGui::Button(n->getDisplayName().c_str())) {
				n->unlock();
			}
		}
	}
	ImGui::End();
}

SaveData TechTree::getSaveData() {
	using namespace SaveKeys;
	SaveData sd(TECH_TREE);
	for (auto kv : TechTree::nodes) {
		if (kv.second.getIsUnlocked()) {
			SaveData tsd(technologyToString(kv.first));
			tsd.addValue(IS_UNLOCKED, true);
			sd.addData(tsd);
		}
	}
	return sd;
}

void TechTree::fromSaveData(SaveData data) {
	for (SaveData d : data.getDatas()) {
		TechTree::nodes.find(stringToTechnology(d.getName()))->second.unlock();
	}
}