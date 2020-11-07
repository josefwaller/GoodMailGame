#include "TechTreeNode.h"

TechTreeNode::TechTreeNode(Technology t, std::string d) : parent(t), isUnlocked(false), displayName(d) {}

Technology TechTreeNode::getParent() {
	return this->parent;
}

std::string TechTreeNode::getDisplayName() {
	return this->displayName;
}

bool TechTreeNode::getIsUnlocked() {
	return this->isUnlocked;
}

void TechTreeNode::unlock() {
	this->isUnlocked = true;
}