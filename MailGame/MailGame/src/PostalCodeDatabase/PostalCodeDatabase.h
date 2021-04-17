#pragma once
#include <SFML/Graphics/Color.hpp>
#include "Constants.h"
#include "PostalCode/PostalCode.h"
#include <map>
#include <vector>

/*
 * Basically a fancy wrapper around a map to hold the postal codes keyed by id.
 */
class PostalCodeDatabase {
public:
	static PostalCodeDatabase* get();

	// Create a new postal code
	id_t createPostalCode(std::string name, sf::Color c = sf::Color::Transparent);
	// Delete a postal code
	void deletePostalCode(id_t id);
	// Get/Set a postal code
	PostalCode getPostalCode(id_t id);
	void setPostalCode(id_t id, PostalCode data);
	// Get the "default" postal code
	// When a tile is no longer in a post office's delivery range, it is set to this postal code
	id_t getDefaultPostalCode();
	// Get the ids of all the postal codes
	std::vector<id_t> getAllIds();
private:
	static PostalCodeDatabase inst;
	// The map of ids->code infos
	std::map<id_t, PostalCode> postalCodes;
	// The current id
	static id_t CURRENT_ID;
};
