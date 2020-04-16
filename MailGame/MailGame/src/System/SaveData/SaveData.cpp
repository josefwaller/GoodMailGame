#include "SaveData.h"
#include <stdexcept>

void SaveData::addData(std::string name, SaveData data) {
	this->values[name] = data;
}
void SaveData::addValue(std::string name, std::string value) {
	this->values[name] = value;
}
std::string SaveData::getValue(std::string name) {
	try {
		return std::get<std::string>(this->values[name]);
	}
	catch (const std::bad_variant_access&) {
		throw std::runtime_error("Tried to get a string value, vbut the value was a SaveData ");
	}
}
SaveData SaveData::getData(std::string name) {
	try {
		return std::get<SaveData>(this->values[name]);
	}
	catch (const std::bad_variant_access&) {
		throw std::runtime_error("Tried to get a SaveData value, but the value was a string");
	}
}
