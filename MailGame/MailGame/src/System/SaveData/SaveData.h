#pragma once
#include <string>
#include <map>
#include <vector>
#include "rapidxml.hpp"
#include "Constants.h"

class SaveData {
public:
	// Create a save data with the name given
	SaveData(std::string name);
	// Load a save data from the file contents
	static SaveData fromFileContents(std::string fileContents);
	// Get the save data represented by the xml node
	static SaveData fromXmlNode(rapidxml::xml_node<>* node);
	// Add a value to the data
	void addValue(std::string name, std::string val);
	// Add an int, long, etc
	void addValue(std::string name, int val);
	void addValue(std::string name, size_t val);
	void addValue(std::string name, bool val);
	void addValue(std::string name, long long val);
	void addValue(std::string name, hour_t val);
	void addValue(std::string name, float val);
	bool hasValue(std::string name);
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
	std::map<std::string, std::string> values;
	// The SaveData that are contained within this data
	std::vector<SaveData> datas;
	// Get the value as a xml node with the name given
	rapidxml::xml_node<>* getDataAsXml(rapidxml::xml_document<>* doc);
};
