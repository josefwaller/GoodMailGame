#pragma once
#include <SFML/System/Vector2.hpp>
class SaveData;

// Parent class for any type of mail, i.e. letters and parcels
class Mail {
public:
	// Current mail id
	static unsigned long long MAIL_ID;

	// Constructor
	Mail(sf::Vector2i dest, sf::Vector2i src);
	// Constructor from save data
	Mail(SaveData d);
	// Compare two Mail objects
	bool operator==(Mail other);
	// Get destination/source
	sf::Vector2i getDest();
	sf::Vector2i getSrc();
	// Get ID
	unsigned long long getId();
	// Get save data
	SaveData getSaveData();
private:
	// The id of the mail
	unsigned long long id;
	// Destination and source
	sf::Vector2i dest;
	sf::Vector2i src;
};
