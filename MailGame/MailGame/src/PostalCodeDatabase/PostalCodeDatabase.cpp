#include "PostalCodeDatabase.h"

PostalCodeDatabase PostalCodeDatabase::inst = PostalCodeDatabase();
id_t PostalCodeDatabase::CURRENT_ID = 0;

PostalCodeDatabase* PostalCodeDatabase::get() {
	return &inst;
}

id_t PostalCodeDatabase::createPostalCode(std::string name, sf::Color c) {
	id_t id = this->CURRENT_ID++;
	PostalCode code = c == sf::Color::Transparent ? PostalCode(id, name) : PostalCode(id, name, c);
	auto x = this->postalCodes.insert({ id, code });
	if (!x.second) {
		x.first->second = code;
	}
	return id;
}

void PostalCodeDatabase::deletePostalCode(id_t id) {
	this->postalCodes.erase(id);
}

void PostalCodeDatabase::setPostalCode(id_t id, PostalCode code) {
	auto x = this->postalCodes.insert({ id, code });
	if (!x.second) {
		x.first->second = code;
	}
}

PostalCode PostalCodeDatabase::getPostalCode(id_t id) {
	return this->postalCodes.at(id);
}

std::vector<id_t> PostalCodeDatabase::getAllIds() {
	std::vector<id_t > toReturn;
	for (id_t i = 0; i < CURRENT_ID; i++) {
		if (this->postalCodes.find(i) != this->postalCodes.end()) {
			toReturn.push_back(i);
		}
	}
	return toReturn;
}

id_t PostalCodeDatabase::getDefaultPostalCode() {
	return this->postalCodes.begin()->first;
}