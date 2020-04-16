#include "SaveData.h"
#include <rapidxml_print.hpp>
#include <stdexcept>
#include <sstream>

SaveData::SaveData() {}
SaveData::SaveData(std::string initData) { //TBA 
}

void SaveData::addData(std::string name, SaveData data) {
	this->values[name] = data;
}
void SaveData::addValue(std::string name, std::string value) {
	this->values[name] = value;
}
std::string SaveData::getValue(std::string name) {
	try {
		return std::get<std::string>(this->values[name]);
	}
	catch (const std::bad_variant_access&) {
		throw std::runtime_error("Tried to get a string value, vbut the value was a SaveData ");
	}
}
SaveData SaveData::getData(std::string name) {
	try {
		return std::get<SaveData>(this->values[name]);
	}
	catch (const std::bad_variant_access&) {
		throw std::runtime_error("Tried to get a SaveData value, but the value was a string");
	}
}
rapidxml::xml_node<>* SaveData::getDataAsXml(rapidxml::xml_document<>* doc, std::string name) {
	// Create node
	rapidxml::xml_node<>* node = doc->allocate_node(
		rapidxml::node_type::node_element,
		doc->allocate_string(name.c_str()));
	// Add nodes for every child
	for (auto it = this->values.begin(); it != this->values.end(); it++) {
		// If it's a string value
		if (std::holds_alternative<std::string>(it->second)) {
			// Add as attribute
			rapidxml::xml_attribute<>* attr = doc->allocate_attribute(
				doc->allocate_string(it->first.c_str()),
				doc->allocate_string(std::get<std::string>(it->second).c_str()));
		}
		else if (std::holds_alternative<SaveData>(it->second)) {
			// Get the node
			rapidxml::xml_node<>* n = std::get<SaveData>(it->second).getDataAsXml(doc, it->first);
			// Add the node to this node
			node->append_node(n);
		}
		else {
			throw std::runtime_error("SaveData has neither a string or a SaveData in its values");
		}
	}
	return node;
}
std::string SaveData::getAsString(std::string name) {
	rapidxml::xml_document<> doc;
	rapidxml::xml_node<>* node = this->getDataAsXml(&doc, name);
	doc.append_node(node);
	std::stringstream ss;
	ss << doc;
	return ss.str();
}
