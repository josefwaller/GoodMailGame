#include "EntityTag.h"
#include <algorithm>

std::string entityTagToString(EntityTag tag) {
	if (tagStrs.find(tag) != tagStrs.end()) {
		return tagStrs.at(tag);
	}
	return "NoStringFound";
}
