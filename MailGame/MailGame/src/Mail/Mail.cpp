#include "Mail/Mail.h"
#include "EventManager/EventManager.h"
#include "Game/Game.h"
#include "System/SaveData/SaveData.h"
#include "Game/Game.h"
#include <algorithm>

unsigned long long Mail::MAIL_ID = 0;
float Mail::percent = 0.0f;
std::map<id_t, std::pair<gtime_t, gtime_t>> Mail::mailRecords = {};
std::map<id_t, MailRecord> Mail::allMail = {};
bool Mail::percentOutdated = false;

Mail::Mail(sf::Vector2i d, sf::Vector2i s, gtime_t time) : id(Mail::MAIL_ID++) {
	// Create new record entry
	Mail::mailRecords[this->id] = { time, 0 };
	Mail::allMail.insert({ this->id, MailRecord(d, s, time) });
	percentOutdated = true;
}

Mail::Mail(SaveData data) {
	using namespace SaveKeys;
	this->id = data.getSizeT(ID);
}

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

bool Mail::operator==(Mail other) {
	return this->id == other.id;
}
sf::Vector2i Mail::getDest() { return this->getMailRecord()->dest; }
sf::Vector2i Mail::getSrc() { return this->getMailRecord()->src; }
unsigned long long Mail::getId() { return this->id; }

void Mail::onDelivery(unsigned long long time, Game* game) {
	game->getEventManager()->onMailDelivered(*this);
	// Set delivery time
	Mail::mailRecords[this->id].second = time;
	updatePercent();
}

void Mail::addEvent(MailEvent e) {
	this->getMailRecord()->mailEvents.push_back(e);
}

bool Mail::hasEvent(MailEvent e) {
	return std::find(this->getMailRecord()->mailEvents.begin(), this->getMailRecord()->mailEvents.end(), e) != this->getMailRecord()->mailEvents.end();
}

SaveData Mail::getSaveData() {
	using namespace SaveKeys;
	SaveData m(MAIL);
	// Just save ID, actual mail data is saved elsewhere
	m.addSizeT(ID, getId());
	return m;
}
void Mail::loadMailRecordData(SaveData mailData) {
	using namespace SaveKeys;
	for (SaveData data : mailData.getDatas()) {
		size_t id = data.getSizeT(ID);
		Mail::MAIL_ID = std::max(Mail::MAIL_ID, id);
		MailRecord record(data.getVector2i(SOURCE), data.getVector2i(DEST), data.getGTimeT(TIME));
		record.mailEvents.resize(data.getSizeT(NUM_EVENTS));
		for (SaveData d : data.getDatas()) {
			if (d.getName() == MAIL_EVENT) {
				record.mailEvents.insert(record.mailEvents.begin() + d.getSizeT(INDEX), (MailEvent)d.getSizeT(MAIL_EVENT));
			}
		}
		this->allMail.insert({ id, record });
	}
}

SaveData Mail::getMailRecordData() {
	using namespace SaveKeys;
	SaveData data(ALL_MAIL);
	for (auto it = allMail.begin(); it != allMail.end(); it++) {
		SaveData d(MAIL_RECORD);
		d.addSizeT(ID, it->first);
		d.addVector2i(SOURCE, it->second.src);
		d.addVector2i(DEST, it->second.dest);
		d.addGTimeT(TIME, it->second.time);
		for (size_t i = 0; i < it->second.mailEvents.size(); i++) {
			SaveData eventData(MAIL_EVENT);
			eventData.addSizeT(INDEX, i);
			eventData.addSizeT(MAIL_EVENT, (size_t)it->second.mailEvents.at(i));
			d.addData(eventData);
		}
		data.addData(d);
	}
	return data;
}

MailRecord* Mail::getMailRecord() {
	return &allMail.find(this->id)->second;
}