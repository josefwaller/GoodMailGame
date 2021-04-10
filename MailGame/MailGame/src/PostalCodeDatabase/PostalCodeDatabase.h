#pragma once
#include <SFML/Graphics/Color.hpp>
#include "Constants.h"
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
	id_t createPostalCode(CodeInfo = {});
	// Delete a postal code
	void deletePostalCode(id_t id);
	// Get/Set a postal code
	CodeInfo getPostalCode(id_t id);
	void setPostalCode(id_t id, CodeInfo data);
	// Get the "default" postal code
	// When a tile is no longer in a post office's delivery range, it is set to this postal code
	id_t getDefaultPostalCode();
	// Get the ids of all the postal codes
	std::vector<id_t> getAllIds();
private:
	static PostalCodeDatabase inst;
	// The map of ids->code infos
	std::map<id_t, CodeInfo> postalCodes;
	// The current id
	static id_t CURRENT_ID;
};
