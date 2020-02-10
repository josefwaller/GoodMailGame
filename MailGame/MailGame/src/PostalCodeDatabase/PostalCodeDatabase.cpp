#include "PostalCodeDatabase.h"

PostalCodeDatabase PostalCodeDatabase::inst = PostalCodeDatabase();
long long PostalCodeDatabase::CURRENT_ID = 0;

PostalCodeDatabase* PostalCodeDatabase::get() {
	return &inst;
}

long long PostalCodeDatabase::createPostalCode(CodeInfo inf) {
	this->postalCodes[this->CURRENT_ID++] = inf;
	return this->CURRENT_ID - 1;
}

void PostalCodeDatabase::deletePostalCode(long long id) {
	this->postalCodes.erase(id);
}

void PostalCodeDatabase::setPostalCode(long long id, CodeInfo inf) {
	this->postalCodes[id] = inf;
}

PostalCodeDatabase::CodeInfo PostalCodeDatabase::getPostalCode(long long id) {
	return this->postalCodes[id];
}

std::vector<long long> PostalCodeDatabase::getAllIds() {
	std::vector<long long> toReturn;
	for (long long i = 0; i < CURRENT_ID; i++) {
		if (this->postalCodes.find(i) != this->postalCodes.end()) {
			toReturn.push_back(i);
		}
	}
	return toReturn;
}
