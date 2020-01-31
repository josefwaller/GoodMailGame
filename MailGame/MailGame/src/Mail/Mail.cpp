#include "Mail/Mail.h"

unsigned long long Mail::MAIL_ID = 0;

Mail::Mail(sf::Vector2i d, sf::Vector2i s) : dest(d), src(s), id(MAIL_ID++) {}
bool Mail::operator==(Mail other) {
	return this->id == other.id;
}
sf::Vector2i Mail::getDest() { return this->dest; }
sf::Vector2i Mail::getSrc() { return this->src; }
unsigned long long Mail::getId() { return this->id; }
