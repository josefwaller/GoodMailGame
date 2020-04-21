#pragma once
#include <string>
#include <map>

#define ALL_COMPONENT_TYPES \
F(Transform) \
F(Renderer) \
F(Controller) \
F(ClickBox) \
F(MailContainer) \
F(Pathfinder) \
F(TransitStop)

#define F(x) x,

// Enum of component types
// Mainly used for saving/loading
enum class ComponentType {
	ALL_COMPONENT_TYPES
};

#undef F
// Method to get a string representation of the component type
std::string componentTypeToStr(ComponentType t);

#define F(x) { ComponentType::x, #x },

// Map of component types to string representations
const std::map<ComponentType, std::string> COMPONENT_STRINGS = {
	ALL_COMPONENT_TYPES
};

#undef F
#undef ALL_COMPONENT_TYPES
