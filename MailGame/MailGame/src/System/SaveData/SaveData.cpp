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
void SaveData::addValue(std::string name, bool val) {
	addValue(name, std::to_string(val));
}
void SaveData::addValue(std::string name, long long val) {
	addValue(name, std::to_string(val));
}
void SaveData::addValue(std::string name, hour_t val) {
	addValue(name, std::to_string(val));
}
void SaveData::addValue(std::string name, float val) {
	addValue(name, std::to_string(val));
}
std::string SaveData::getValue(std::string name) {
	return this->values[name];
}
float SaveData::getValuef(std::string name) {
	return std::stof(getValue(name));
}
std::vector<SaveData> SaveData::getDatas() {
	return this->datas;
}
bool SaveData::hasValue(std::string name) {
	return (this->values.find(name) != this->values.end());
}
void SaveData::addValuesFrom(SaveData other) {
	for (std::string key : other.getValueNames()) {
		this->addValue(key, other.getValue(key));
	}
}
std::vector<std::string> SaveData::getValueNames() {
	std::vector<std::string> toReturn;
	for (auto it = this->values.begin(); it != this->values.end(); it++) {
		toReturn.push_back(it->first);
	}
	return toReturn;
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

SaveData SaveData::getData(std::string name) {
	for (auto d : this->datas) {
		if (d.name == name) {
			return d;
		}
	}
	return SaveData("");
}
SaveData SaveData::fromFileContents(std::string data) {
	rapidxml::xml_document<> doc;
	// Unsafe stuff going on here
	char* c = new char[data.length() + 1];
	strncpy_s(c, data.length() + 1, data.c_str(), data.length());
	doc.parse<0>(c);
	// Load all the stuff
	rapidxml::xml_node<>* n = doc.first_node();
	SaveData toReturn = SaveData::fromXmlNode(n);

	delete[] c;
	return toReturn;
}
SaveData SaveData::fromXmlNode(rapidxml::xml_node<>* node) {
	// Create save data
	SaveData toReturn(std::string(node->name()));
	// Add values
	for (rapidxml::xml_attribute<>* attr = node->first_attribute(); attr != NULL; attr = attr->next_attribute()) {
		toReturn.addValue(std::string(attr->name()), std::string(attr->value()));
	}
	// Add nexted datas
	for (rapidxml::xml_node<>* n = node->first_node(); n != NULL; n = n->next_sibling()) {
		toReturn.addData(fromXmlNode(n));
	}
	return toReturn;
}
std::string SaveData::getAsString() {
	rapidxml::xml_document<> doc;
	rapidxml::xml_node<>* node = this->getDataAsXml(&doc);
	doc.append_node(node);
	std::stringstream ss;
	ss << doc;
	return ss.str();
}

std::string SaveData::getName() { return this->name; }
