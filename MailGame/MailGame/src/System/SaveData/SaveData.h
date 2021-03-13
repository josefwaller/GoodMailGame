#pragma once
#include <string>
#include <map>
#include <vector>
#include "rapidxml.hpp"
#include "Constants.h"
#include <type_traits>

class IsoRotation;

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
	/* these are all being removed */
	void addValue(sdkey_t k, std::string v) {
		this->values.insert({ k, v });
	}
	// Add an int, long, etc
	void addValue(std::string name, int val);
	void addValue(std::string name, size_t val);
	void addValue(std::string name, bool val);
	void addValue(std::string name, long long val);
	void addValue(std::string name, hour_t val);
	void addValue(std::string name, float val);
	// Add a nested SaveData
	void addData(SaveData data);
	// Copy all the values of the SaveData
	void addValuesFrom(SaveData data);
	// Get a value/dta
	std::string getValue(std::string name);
	float getValuef(std::string name);
	std::vector<SaveData> getDatas();
	// Get the first data with the name given
	SaveData getData(std::string name);
	/* these are the new ways to do it */
	void addString(sdkey_t key, std::string value);
	void addSizeT(sdkey_t key, size_t value);
	void addFloat(sdkey_t key, float value);
	void addBool(sdkey_t key, bool value);
	void addIsoRotation(sdkey_t key, IsoRotation value);

	std::string getString(sdkey_t);
	size_t getSizeT(sdkey_t);
	float getFloat(sdkey_t);
	bool getBool(sdkey_t);
	IsoRotation getIsoRotation(sdkey_t key);

	// check if a value exists
	bool hasValue(sdkey_t name);
	// Get the string to save to file
	std::string getAsString();
	// Get the name
	std::string getName();
	// Get the names of the values
	std::vector<std::string> getValueNames();
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
