#include "SaveData.h"
#include "System/IsoRotation/IsoRotation.h"
#include "Routes/RoutePoint.h"
#include "Entity/Entity.h"
#include "Component/Controller/VehicleController/VehicleController.h"
#include "Game/Game.h"
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
std::vector<SaveData> SaveData::getDatas() {
	return this->datas;
}
bool SaveData::hasValue(sdkey_t name) {
	return (this->values.find(name) != this->values.end());
}
void SaveData::addValuesFrom(SaveData other) {
	for (sdkey_t key : other.getKeys()) {
		this->addString(key, other.getString(key));
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
void SaveData::addRoutePointVector(sdkey_t key, std::vector<RoutePoint> points) {
	SaveData data(key);
	data.addSizeT(SaveKeys::SIZE, points.size());
	for (size_t i = 0; i < points.size(); i++) {
		SaveData rD = routePointToSaveData(points.at(i));
		rD.addSizeT(SaveKeys::INDEX, i);
		data.addData(rD);
	}
	this->addData(data);
}
void SaveData::addVector2iVector(sdkey_t key, std::vector<sf::Vector2i> value) {
	using namespace SaveKeys;
	SaveData data(key);
	data.addSizeT(SaveKeys::SIZE, value.size());
	for (size_t i = 0; i < value.size(); i++) {
		SaveData d(VECTOR_2I);;
		d.addSizeT(INDEX, i);
		d.addInt(X, value[i].x);
		d.addInt(Y, value[i].y);
		data.addData(d);
	}
	this->addData(data);
}

void SaveData::addVehicleControllerStopVector(sdkey_t key, std::vector<VehicleControllerStop> values) {
	using namespace SaveKeys;
	SaveData data(key);
	data.addSizeT(SIZE, values.size());
	for (size_t i = 0; i < values.size(); i++) {
		VehicleControllerStop s = values[i];
		SaveData d(VEHICLE_CONTROLLER_STOP);
		d.addSizeT(INDEX, i);
		d.addGTimeT(EXPECTED_TIME, s.expectedTime);
		d.addFloat(DISTANCE, s.distance);
		d.addGTimeT(WAIT_TIME, s.waitTime);
		if (s.hasEntityTarget()) {
			d.addBool(HAS_ENTITY, true);
			if (s.getEntityTarget().lock()) {
				d.addSizeT(ENTITY_ID, s.getEntityTarget().lock()->getId());
			}
		}
		else {
			d.addBool(HAS_ENTITY, false);
			d.addVector2i(TILE, s.getTileTarget());
		}
		data.addData(d);
	}
	this->addData(data);
}

std::vector<VehicleControllerStop> SaveData::getVehicleControllerStopVector(sdkey_t key, Game* g) {
	using namespace SaveKeys;
	SaveData data = this->getData(key);
	std::vector<VehicleControllerStop> stops;
	stops.resize(data.getSizeT(SIZE));
	for (SaveData d : data.getDatas()) {
		size_t expectedTime = d.getGTimeT(EXPECTED_TIME);
		float distance = d.getFloat(DISTANCE);
		VehicleControllerStop s;
		if (d.getBool(HAS_ENTITY)) {
			s = VehicleControllerStop(g->getEntityById(d.getSizeT(ENTITY_ID)), d.getGTimeT(WAIT_TIME));
		}
		else {
			s = VehicleControllerStop(d.getVector2i(TILE), d.getGTimeT(WAIT_TIME));
		}
		s.expectedTime = expectedTime;
		s.distance = distance;
		stops[d.getSizeT(INDEX)] = s;
	}
	return stops;
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
std::vector<RoutePoint> SaveData::getRoutePointVector(sdkey_t key) {
	SaveData data = this->getData(key);
	std::vector<RoutePoint> points;
	points.resize(data.getSizeT(SaveKeys::SIZE));
	for (SaveData rd : data.getDatas()) {
		points[rd.getSizeT(SaveKeys::INDEX)] = saveDataToRoutePoint(rd);
	}
	return points;
}

std::vector<sf::Vector2i> SaveData::getVector2iVector(sdkey_t key) {
	using namespace SaveKeys;
	SaveData data = this->getData(key);
	std::vector<sf::Vector2i> values;
	values.resize(data.getSizeT(SIZE));
	for (SaveData d : data.getDatas()) {
		values[d.getSizeT(INDEX)] = sf::Vector2i(d.getInt(X), d.getInt(Y));
	}
	return values;
}

std::vector<std::string> SaveData::getKeys() {
	std::vector<sdkey_t> toReturn;
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
		toReturn.addString(std::string(attr->name()), std::string(attr->value()));
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