#pragma once
#include <string>
#include <map>
#include <vector>
#include "rapidxml.hpp"
#include "Constants.h"
#include <type_traits>
#include <SFML/System/Vector2.hpp>
#include <SFML/System/Vector3.hpp>

class IsoRotation;
class RoutePoint;
class VehicleControllerStop;
class Game;

class SaveData {
public:
	// Create a save data with the name given
	SaveData(std::string name);
	// Copy an existing save data with a new name
	// Generally used for children classes
	SaveData(std::string name, SaveData data);
	// Load a save data from the file contents
	static SaveData fromFileContents(std::string fileContents);
	// Get the save data represented by the xml node
	static SaveData fromXmlNode(rapidxml::xml_node<>* node);
	// Add a nested SaveData
	void addData(SaveData data);
	// Copy all the values of the SaveData
	void addValuesFrom(SaveData data);
	std::vector<SaveData> getDatas();
	// Get the first data with the name given
	SaveData getData(sdkey_t name);
	/* these are the new ways to do it */
	void addString(sdkey_t key, std::string value);
	void addInt(sdkey_t key, int value);
	void addSizeT(sdkey_t key, size_t value);
	void addFloat(sdkey_t key, float value);
	void addBool(sdkey_t key, bool value);
	void addIsoRotation(sdkey_t key, IsoRotation value);
	void addVector2i(sdkey_t key, sf::Vector2i value);
	void addVector3f(sdkey_t key, sf::Vector3f value);
	void addHourT(sdkey_t key, hour_t value);
	void addGTimeT(sdkey_t key, gtime_t value);
	void addRoutePointVector(sdkey_t key, std::vector<RoutePoint> value);
	void addVehicleControllerStopVector(sdkey_t key, std::vector<VehicleControllerStop> value);
	void addVector2iVector(sdkey_t key, std::vector<sf::Vector2i> value);

	std::string getString(sdkey_t);
	size_t getSizeT(sdkey_t);
	int getInt(sdkey_t);
	float getFloat(sdkey_t);
	bool getBool(sdkey_t);
	IsoRotation getIsoRotation(sdkey_t);
	sf::Vector2i getVector2i(sdkey_t);
	sf::Vector3f getVector3f(sdkey_t);
	hour_t getHourT(sdkey_t);
	gtime_t getGTimeT(sdkey_t);
	std::vector<RoutePoint> getRoutePointVector(sdkey_t);
	// This method will eventually be removed
	std::vector<VehicleControllerStop> getVehicleControllerStopVector(sdkey_t, Game*);
	std::vector<sf::Vector2i> getVector2iVector(sdkey_t);
	// Check if a value exists
	bool hasValue(sdkey_t);
	// Get the string to save to file
	std::string getAsString();
	// Get the name
	std::string getName();
	// Get the names of the values
	std::vector<sdkey_t> getKeys();
private:
	// The name of this data
	std::string name;
	// The values of this data
	std::map<sdkey_t, std::string> values;
	// The SaveData that are contained within this data
	std::vector<SaveData> datas;
	// Get the value as a xml node with the name given
	rapidxml::xml_node<>* getDataAsXml(rapidxml::xml_document<>* doc);
};
