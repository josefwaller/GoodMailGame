#include "ComponentType.h"

std::string componentTypeToStr(ComponentType t) {
	if (COMPONENT_STRINGS.find(t) != COMPONENT_STRINGS.end()) {
		return COMPONENT_STRINGS.at(t);
	}
	return "UnknownComponentType";
}

ComponentType strToComponentType(std::string str) {
	if (STRING_COMPONENTS.find(str) != STRING_COMPONENTS.end()) {
		return STRING_COMPONENTS.at(str);
	}
	return ComponentType::Transform;
}
