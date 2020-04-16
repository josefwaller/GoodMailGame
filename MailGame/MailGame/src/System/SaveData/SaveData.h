#pragma once
#include <string>
#include <map>
#include <vector>
#include "rapidxml.hpp"

class SaveData {
public:
	// Create a save data with the name given
	SaveData(std::string name);
	// Add a value to the data
	void addValue(std::string name, std::string val);
	// Add a nested SaveData
	void addData(SaveData data);
	// Get a value/dta
	std::string getValue(std::string name);
	std::vector<SaveData> getDatas();
	// Get the string to save to file
	std::string getAsString();
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
