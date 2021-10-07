#pragma once
#include "System/IsoRotation/IsoRotation.h"
#include <vector>

class SaveData;

// The railway on a tile
class Railway {
public:
	// Whether the railway is a station or not
	bool isStation;
	Railway(IsoRotation from, IsoRotation to, bool isOneWay = true, bool isStation = false);
	Railway(SaveData data);
	// Get directions
	IsoRotation getFrom();
	IsoRotation getTo();
	// Get whether the railway is one way or two way
	bool getIsOneWay();

	// Get whether the railway is locked
	bool getIsLocked();
	// Get/release lock
	void getLock();
	void releaseLock();

	// Get save data
	SaveData getSaveData();
	bool operator==(Railway other);
private:
	IsoRotation from;
	IsoRotation to;
	// The directions the Railway goes in
	// i.e. { North, South } means that the railway goes from the north to the south
	std::vector<std::pair<IsoRotation, IsoRotation>> directions;
	// Whether the railway is locked or not
	bool isLocked;
	bool isOneWay;
};
