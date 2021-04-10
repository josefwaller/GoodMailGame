#include "PostalCodeDatabase.h"

PostalCodeDatabase PostalCodeDatabase::inst = PostalCodeDatabase();
id_t PostalCodeDatabase::CURRENT_ID = 0;

PostalCodeDatabase* PostalCodeDatabase::get() {
	return &inst;
}

id_t PostalCodeDatabase::createPostalCode(CodeInfo inf) {
	this->postalCodes[this->CURRENT_ID++] = inf;
	return this->CURRENT_ID - 1;
}

void PostalCodeDatabase::deletePostalCode(id_t id) {
	this->postalCodes.erase(id);
}

void PostalCodeDatabase::setPostalCode(id_t id, CodeInfo inf) {
	this->postalCodes[id] = inf;
}

PostalCodeDatabase::CodeInfo PostalCodeDatabase::getPostalCode(id_t id) {
	return this->postalCodes[id];
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