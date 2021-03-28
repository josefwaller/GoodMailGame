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

// Parent class for any type of mail, i.e. letters and parcels
class Mail {
public:
	// Current mail id
	static unsigned long long MAIL_ID;
	// Map of mail ids -> pair<creation time, delivery time>
	// If delivery time is 0, it means it has not been delivered
	static std::map<id_t, std::pair<gtime_t, gtime_t>> mailRecords;
	// Get percentage of letters in the last 24 hrs delivered
	static float getPercentDelivered();
	// Should be called on hour changed to remove records of letters we don't care about
	static void onTimeChanged(time_t time);

	// Constructor
	Mail(sf::Vector2i dest, sf::Vector2i src, unsigned long long time);
	// Constructor from save data
	Mail(SaveData d);
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
	// Update the percentage of mail delivered
	static void updatePercent();
	static float percent;
	// Whether percent needs to be updated
	static bool percentOutdated;
	// The id of the mail
	unsigned long long id;
	// Destination and source
	sf::Vector2i dest;
	sf::Vector2i src;
	// List of events that have happened to this letter
	std::vector<MailEvent> events;
};
