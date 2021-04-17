#pragma once
#include <SFML/Graphics/Color.hpp>
#include "Constants.h"

class PostalCode {
public:
	PostalCode(id_t, std::string);
	PostalCode(id_t, std::string, sf::Color);
	id_t getId();
	sf::Color getColor();
	void setColor(sf::Color c);
	std::string getName();
	void setName(std::string n);

private:
	id_t id;
	sf::Color color;
	std::string name;
};
