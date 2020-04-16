#pragma once
#include <string>
#include <map>
#include <variant>

class SaveData {
public:
	// Add a value to the data
	void addValue(std::string name, std::string val);
	// Add a nested SaveData
	void addData(std::string name, SaveData data);
	// Get a value/dta
	std::string getValue(std::string name);
	SaveData getData(std::string name);
private:
	// The values/data in the save data
	std::map<std::string, std::variant<std::string, SaveData>> values;
};
