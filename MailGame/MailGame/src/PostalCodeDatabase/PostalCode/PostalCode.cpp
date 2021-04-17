#include "PostalCode.h"

PostalCode::PostalCode(id_t i, std::string n) : id(i), name(n) {
	// Generate random color
	this->color = sf::Color(rand() % 256, rand() % 256, rand() % 256, 100);
}
PostalCode::PostalCode(id_t i, std::string n, sf::Color c) : id(i), name(n), color(c) {}

sf::Color PostalCode::getColor() {
	return this->color;
}
void PostalCode::setColor(sf::Color c) {
	this->color = c;
}
std::string PostalCode::getName() {
	return this->name;
}
void PostalCode::setName(std::string n) {
	this->name = n;
}
id_t PostalCode::getId() {
	return this->id;
}