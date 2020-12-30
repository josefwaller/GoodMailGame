#include "Mail/Mail.h"
#include "System/SaveData/SaveData.h"
#include "Game/Game.h"
#include <algorithm>

unsigned long long Mail::MAIL_ID = 0;
float Mail::percent = 0.0f;
std::map<id_t, std::pair<gtime_t, gtime_t>> Mail::mailRecords = {};
bool Mail::percentOutdated = false;

float Mail::getPercentDelivered() {
	if (percentOutdated) {
		updatePercent();
		percentOutdated = false;
	}
	return percent;
}
void Mail::updatePercent() {
	unsigned long long delivered = 0, total = 0;
	for (auto kv : mailRecords) {
		if (kv.second.second != 0 && kv.second.second - kv.second.first <= 24) {
			delivered++;
		}
		total++;
	}
	percent = (float)delivered / (float)total;
}

void Mail::onTimeChanged(time_t time) {
	// Remove mail
	std::vector<id_t> toRemove;
	for (auto kv : mailRecords) {
		if (time - kv.second.first >= 24 * Game::UNITS_IN_GAME_HOUR) {
			toRemove.push_back(kv.first);
		}
	}
	for (auto id : toRemove) {
		mailRecords.erase(id);
	}
	percentOutdated = true;
}

Mail::Mail(sf::Vector2i d, sf::Vector2i s, unsigned long long time) : dest(d), src(s), id(MAIL_ID++) {
	// Create new record entry
	Mail::mailRecords[this->id] = { time, 0 };
	percentOutdated = true;
}
bool Mail::operator==(Mail other) {
	return this->id == other.id;
}
sf::Vector2i Mail::getDest() { return this->dest; }
sf::Vector2i Mail::getSrc() { return this->src; }
unsigned long long Mail::getId() { return this->id; }

void Mail::onDelivery(unsigned long long time) {
	// Set delivery time
	Mail::mailRecords[this->id].second = time;
	updatePercent();
}

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