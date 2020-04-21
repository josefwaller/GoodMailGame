#include "ComponentType.h"

std::string componentTypeToStr(ComponentType t) {
	if (COMPONENT_STRINGS.find(t) != COMPONENT_STRINGS.end()) {
		return COMPONENT_STRINGS.at(t);
	}
	return "UnknownComponentType";
}
