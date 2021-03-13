#pragma once
#include <string>
#include <map>
#include <memory>

class Component;
class Entity;

#define ALL_COMPONENT_TYPES \
F(Transform) \
F(Renderer) \
F(Controller) \
F(ClickBox) \
F(MailContainer) \
F(Pathfinder) \
F(TransitStop) \
F(Ai)

#define F(x) x,

// Enum of component types
// Mainly used for saving/loading
enum class ComponentType {
	ALL_COMPONENT_TYPES
};

#undef F
// Method to get a string representation of the component type
std::string componentTypeToStr(ComponentType t);
// Get the component type from the string given
ComponentType strToComponentType(std::string str);

#define F(x) { ComponentType::x, #x },

// Map of component types to string representations
const std::map<ComponentType, std::string> COMPONENT_STRINGS = {
	ALL_COMPONENT_TYPES
};
#undef F
#define F(x) { #x, ComponentType::x },
// Map of strings to component types
const std::map<std::string, ComponentType> STRING_COMPONENTS = {
	ALL_COMPONENT_TYPES
};
#undef F
#undef ALL_COMPONENT_TYPES
