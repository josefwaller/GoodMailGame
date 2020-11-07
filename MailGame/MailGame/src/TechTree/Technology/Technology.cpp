#include "Technology.h"
#include <stdexcept>

std::string technologyToString(Technology t) {
	return TECH_STRS.find(t)->second;
}

Technology stringToTechnology(std::string s) {
	for (auto it = TECH_STRS.begin(); it != TECH_STRS.end(); it++) {
		if (it->second == s) {
			return it->first;
		}
	}
	throw std::runtime_error("No technology found for string " + s);
}