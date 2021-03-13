#include "SaveData.h"
#include "System/IsoRotation/IsoRotation.h"
#include <rapidxml_print.hpp>
#include <stdexcept>
#include <sstream>

#ifdef _DEBUG
#define ADD_VALUE(a, b) \
if (this->values.find(a) != this->values.end()) { \
	throw std::runtime_error("Keys not unique!"); \
} \
this->values.insert({ a, std::to_string(b) });
#else
#define ADD_VALUE(a, b) this->values.insert({ a, std::to_string(b)});
#endif
SaveData::SaveData(std::string name) : name(name) {}

SaveData::SaveData(std::string name, SaveData data) : name(name), values(data.values) {};

void SaveData::addData(SaveData data) {
	this->datas.push_back(data);
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

void SaveData::addString(sdkey_t key, std::string value) {
	this->values.insert({ key, value });
}
void SaveData::addInt(sdkey_t key, int value) {
	ADD_VALUE(key, value);
}
void SaveData::addFloat(sdkey_t key, float value) {
	ADD_VALUE(key, value);
}
void SaveData::addSizeT(sdkey_t key, size_t value) {
	ADD_VALUE(key, value);
}
void SaveData::addHourT(sdkey_t key, hour_t hour) {
	addSizeT(key, hour);
}
void SaveData::addGTimeT(sdkey_t key, gtime_t t) {
	addSizeT(key, t);
}
void SaveData::addBool(sdkey_t key, bool value) {
	ADD_VALUE(key, value);
}
void SaveData::addIsoRotation(sdkey_t key, IsoRotation rot) {
	ADD_VALUE(key, rot.getRotation());
}
void SaveData::addVector2i(sdkey_t key, sf::Vector2i value) {
	SaveData data(key);
	data.addInt(SaveKeys::X, value.x);
	data.addInt(SaveKeys::Y, value.y);
	this->addData(data);
}
void SaveData::addVector3f(sdkey_t key, sf::Vector3f value) {
	using namespace SaveKeys;
	SaveData data(key);
	data.addFloat(X, value.x);
	data.addFloat(Y, value.y);
	data.addFloat(Z, value.z);
	this->addData(data);
}

std::string SaveData::getString(sdkey_t key) {
	return this->values.at(key);
}
int SaveData::getInt(sdkey_t key) {
	return std::stoi(this->values.at(key));
}
float SaveData::getFloat(sdkey_t key) {
	return std::stof(this->values.at(key));
}
size_t SaveData::getSizeT(sdkey_t key) {
	return (size_t)std::stoull(this->values.at(key));
}
bool SaveData::getBool(sdkey_t key) {
	return this->values.at(key) == "1";
}
IsoRotation SaveData::getIsoRotation(sdkey_t key) {
	return IsoRotation((unsigned int)std::stoul(this->values.at(key)));
}
sf::Vector2i SaveData::getVector2i(sdkey_t key) {
	SaveData d = this->getData(key);
	return sf::Vector2i(d.getInt(SaveKeys::X), d.getInt(SaveKeys::Y));
}
sf::Vector3f SaveData::getVector3f(sdkey_t key) {
	using namespace SaveKeys;
	SaveData d = this->getData(key);
	return sf::Vector3f(d.getFloat(X), d.getFloat(Y), d.getFloat(Z));
}
gtime_t SaveData::getGTimeT(sdkey_t key) {
	return (gtime_t)getSizeT(key);
}
hour_t SaveData::getHourT(sdkey_t key) {
	return (hour_t)getSizeT(key);
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

#undef ADD_VALUE;