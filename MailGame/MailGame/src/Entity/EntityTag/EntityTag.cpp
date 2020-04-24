#include "EntityTag.h"
#include <algorithm>

std::string entityTagToString(EntityTag tag) {
	if (tagStrs.find(tag) != tagStrs.end()) {
		return tagStrs.at(tag);
	}
	return "NoStringFound";
}

EntityTag strToEntityTag(std::string str) {
	if (strTags.find(str) != strTags.end()) {
		return strTags.at(str);
	}
	return EntityTag::PostOffice; // idk what to do here ?
}

std::shared_ptr<Entity> entityTagToEntity(EntityTag tag, Game* g, sf::Vector2f pos, IsoRotation rot) {
	return loadEntityFuncs.at(tag)(g, pos, rot);
}
