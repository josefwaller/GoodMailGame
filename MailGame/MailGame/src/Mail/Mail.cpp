#include "Mail/Mail.h"

Mail::Mail(sf::Vector2i d, sf::Vector2i s) : dest(d), src(s) {}
bool Mail::operator==(Mail other) {
	return this->dest == other.dest && this->src == other.src;
}
