#pragma once
#include "Mail/Mail.h"
#include <SFML/System/Vector2.hpp>

class Letter : public Mail {
public:
	sf::Vector2i dest;
	sf::Vector2i src;
};
