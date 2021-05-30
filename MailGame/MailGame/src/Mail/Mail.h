#pragma once
#include <SFML/System/Vector2.hpp>
#include <map>
#include <utility>
#include <vector>
#include "Constants.h"
class SaveData;
class Game;

// Different things that can happen to mail
// Tracked for reasons (i.e. unlocing new technology)
enum class MailEvent {
	OnMailTruck,
	OnCargoTruck,
	OnTrain,
	OnAirplane
};

struct MailRecord {
	sf::Vector2i dest;
	sf::Vector2i src;
	std::vector<MailEvent> mailEvents;
	gtime_t time;
	bool hasBeenDelivered;
	gtime_t deliveryTime;
	MailRecord(sf::Vector2i src, sf::Vector2i dest, gtime_t t) : dest(dest), src(src), mailEvents({}), time(t), hasBeenDelivered(false), deliveryTime(0) {}
};

// Parent class for any type of mail, i.e. letters and parcels
class Mail {
public:
	// Current mail id
	static unsigned long long MAIL_ID;
	// Should be called on hour changed to remove records of letters we don't care about
	static void onTimeChanged(time_t time);
	// Save/Load the mail records
	static SaveData getMailRecordData();
	static void loadMailRecordData(SaveData d);

	// Get all the mail records
	static std::map<id_t, MailRecord> getAllMail();

	// Constructor
	// Really just creates a fancy pointer by id to the MailRecord
	Mail(sf::Vector2i src, sf::Vector2i dest, gtime_t);
	// Constructor from save data
	Mail(SaveData d);
	// Check if mail has expired
	bool hasExpired();
	// Add event
	void addEvent(MailEvent e);
	// Find if the letter has a certain event
	bool hasEvent(MailEvent e);
	// Compare two Mail objects
	bool operator==(Mail other);
	// Get destination/source
	sf::Vector2i getDest();
	sf::Vector2i getSrc();
	// Get ID
	unsigned long long getId();
	// Method triggered on delivery
	void onDelivery(unsigned long long time, Game* g);
	// Get save data
	SaveData getSaveData();
private:
	// All the mail records that exist
	static std::map<id_t, MailRecord> allMail;
	// The id of the mail
	// Used to find the MailRecord in the table
	id_t id;
	// Get the mail record
	MailRecord* getMailRecord();
};
