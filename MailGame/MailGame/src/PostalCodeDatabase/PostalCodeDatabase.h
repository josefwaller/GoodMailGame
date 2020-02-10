#pragma once
#include <SFML/Graphics/Color.hpp>
#include <map>
#include <vector>

/*
 * Basically a fancy wrapper around a map to hold the postal codes keyed by id.
 */
class PostalCodeDatabase {
public:
	struct CodeInfo {
		// The color to draw the code
		sf::Color color;
	};

	static PostalCodeDatabase* get();

	// Create a new postal code
	long long createPostalCode(CodeInfo = {});
	// Delete a postal code
	void deletePostalCode(long long id);
	// Get/Set a postal code
	CodeInfo getPostalCode(long long id);
	void setPostalCode(long long id, CodeInfo data);
	// Get the ids of all the postal codes
	std::vector<long long> getAllIds();
private:
	static PostalCodeDatabase inst;
	// The map of ids->code infos
	std::map<long long, CodeInfo> postalCodes;
	// The current id
	static long long CURRENT_ID;
};
