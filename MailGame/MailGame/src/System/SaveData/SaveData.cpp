#include "SaveData.h"
#include <rapidxml_print.hpp>
#include <stdexcept>
#include <sstream>

SaveData::SaveData(std::string name): name(name) {}

void SaveData::addData(SaveData data) {
	this->datas.push_back(data);
}
void SaveData::addValue(std::string name, std::string value) {
	this->values[name] = value;
}
void SaveData::addValue(std::string name, int value) {
	addValue(name, std::to_string(value));
}
void SaveData::addValue(std::string name, size_t value) {
	addValue(name, std::to_string(value));
}
std::string SaveData::getValue(std::string name) {
	return this->values[name];
}
std::vector<SaveData> SaveData::getDatas() {
	return this->datas;
}
rapidxml::xml_node<>* SaveData::getDataAsXml(rapidxml::xml_document<>* doc) {
	// Create node
	rapidxml::xml_node<>* node = doc->allocate_node(
		rapidxml::node_type::node_element,
		doc->allocate_string(this->name.c_str()));
	// Add attributes for values
	for (auto kv : this->values) {
		rapidxml::xml_attribute<>* attr = doc->allocate_attribute(
			doc->allocate_string(kv.first.c_str()),
			doc->allocate_string(kv.second.c_str())
		);
		node->append_attribute(attr);
	}
	// Add nodes for datas
	for (SaveData d : this->datas) {
		rapidxml::xml_node<>* n = d.getDataAsXml(doc);
		node->append_node(n);
	}
	return node;
}
std::string SaveData::getAsString() {
	rapidxml::xml_document<> doc;
	rapidxml::xml_node<>* node = this->getDataAsXml(&doc);
	doc.append_node(node);
	std::stringstream ss;
	ss << doc;
	return ss.str();
}
