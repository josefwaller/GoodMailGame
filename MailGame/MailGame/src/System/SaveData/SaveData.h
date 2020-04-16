#pragma once
#include <string>
#include <map>
#include <variant>
#include "rapidxml.hpp"

class SaveData {
public:
	// Create an empty save data
	SaveData();
	// Create a save data from existing xml
	SaveData(std::string data);
	// Add a value to the data
	void addValue(std::string name, std::string val);
	// Add a nested SaveData
	void addData(std::string name, SaveData data);
	// Get a value/dta
	std::string getValue(std::string name);
	SaveData getData(std::string name);
	// Get the string to save to file
	std::string getAsString(std::string name);
private:
	// The values/data in the save data
	std::map<std::string, std::variant<std::string, SaveData>> values;
	// Get the value as a xml node with the name given
	rapidxml::xml_node<>* getDataAsXml(rapidxml::xml_document<>* doc, std::string name);
};
