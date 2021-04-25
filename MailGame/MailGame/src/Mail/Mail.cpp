#include "Mail/Mail.h"
#include "EventManager/EventManager.h"
#include "Game/Game.h"
#include "System/SaveData/SaveData.h"
#include "Game/Game.h"
#include <algorithm>

unsigned long long Mail::MAIL_ID = 0;
std::map<id_t, MailRecord> Mail::allMail = {};

std::map<id_t, MailRecord> Mail::getAllMail() {
	return Mail::allMail;
}

Mail::Mail(sf::Vector2i s, sf::Vector2i d, gtime_t time) : id(Mail::MAIL_ID++) {
	// Create new record entry
	Mail::allMail.insert({ this->id, MailRecord(s, d, time) });
}

Mail::Mail(SaveData data) {
	using namespace SaveKeys;
	this->id = data.getSizeT(ID);
}

void Mail::onTimeChanged(time_t time) {
	// Remove mail
	std::vector<id_t> toRemove;
	for (auto kv : allMail) {
		if (time - kv.second.time >= 48 * Game::UNITS_IN_GAME_HOUR && kv.second.hasBeenDelivered) {
			toRemove.push_back(kv.first);
		}
	}
	for (auto id : toRemove) {
		allMail.erase(id);
	}
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
	this->getMailRecord()->hasBeenDelivered = true;
	this->getMailRecord()->deliveryTime = time;
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
		record.hasBeenDelivered = data.getBool(IS_DELIVERED);
		record.deliveryTime = data.getGTimeT(DELIVERY_TIME);
		Mail::allMail.insert({ id, record });
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
		d.addSizeT(NUM_EVENTS, it->second.mailEvents.size());
		d.addBool(IS_DELIVERED, it->second.hasBeenDelivered);
		d.addGTimeT(DELIVERY_TIME, it->second.deliveryTime);
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