#include "Mail/Mail.h"
#include "System/SaveData/SaveData.h"
#include <algorithm>

unsigned long long Mail::MAIL_ID = 0;

Mail::Mail(sf::Vector2i d, sf::Vector2i s) : dest(d), src(s), id(MAIL_ID++) {}
bool Mail::operator==(Mail other) {
	return this->id == other.id;
}
sf::Vector2i Mail::getDest() { return this->dest; }
sf::Vector2i Mail::getSrc() { return this->src; }
unsigned long long Mail::getId() { return this->id; }

SaveData Mail::getSaveData() {
	SaveData m("Mail");
	m.addValue("id", std::to_string(getId()));
	m.addValue("srcX", std::to_string(getSrc().x));
	m.addValue("srcY", std::to_string(getSrc().y));
	m.addValue("destX", std::to_string(getDest().x));
	m.addValue("destY", std::to_string(getDest().y));
	return m;
}
Mail::Mail(SaveData data) {
	this->id = std::stoull(data.getValue("id"));
	Mail::MAIL_ID = std::max(Mail::MAIL_ID, this->id);
	this->src = sf::Vector2i(
		std::stoi(data.getValue("srcX")),
		std::stoi(data.getValue("srcY"))
	);
	this->dest = sf::Vector2i(
		std::stoi(data.getValue("destX")),
		std::stoi(data.getValue("destY"))
	);
}
