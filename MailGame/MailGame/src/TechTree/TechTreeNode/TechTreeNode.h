#pragma once
#include "TechTree/Technology/Technology.h"
#include <string>

class TechTreeNode {
public:
	TechTreeNode(Technology t, std::string displayName);
	Technology getParent();
	bool getIsUnlocked();
	void unlock();
	std::string getDisplayName();
private:
	Technology parent;
	bool isUnlocked;
	std::string displayName;
};
