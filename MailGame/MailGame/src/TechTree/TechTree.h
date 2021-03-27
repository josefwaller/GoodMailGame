#pragma once
#include "TechTreeNode/TechTreeNode.h"
#include "System/SaveData/SaveData.h"
#include <map>

class TechTree {
public:
	TechTree();
	TechTree(SaveData data);
	bool getTechUnlocked(Technology tech);
	void unlockTech(Technology tech);
	// Currently this just draws the UI
	void update();
	SaveData getSaveData();
private:
	// Map of technologies to relevant tech tree node
	std::map<Technology, TechTreeNode> nodes;
};
