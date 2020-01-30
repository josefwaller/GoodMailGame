#pragma once
#include <SFML/System/Vector2.hpp>

// Parent class for any type of mail, i.e. letters and parcels
class Mail {
public:
	sf::Vector2i dest;
	sf::Vector2i src;
	bool operator==(Mail other);
};
