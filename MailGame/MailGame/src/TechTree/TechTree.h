#pragma once
#include "TechTreeNode/TechTreeNode.h"
#include "System/SaveData/SaveData.h"
#include <map>

class TechTree {
public:
	static bool getTechUnlocked(Technology tech);
	static void unlockTech(Technology tech);
	// Currently this just draws the UI
	static void update();
	static SaveData getSaveData();
	static void fromSaveData(SaveData data);
private:
	// Map of technologies to relevant tech tree node
	static std::map<Technology, TechTreeNode> nodes;
};
