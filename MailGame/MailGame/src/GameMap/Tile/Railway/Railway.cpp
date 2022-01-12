#include "Railway.h"
#include "Constants.h"
#include "System/SaveData/SaveData.h"
#include <algorithm>

Railway::Railway(IsoRotation f, IsoRotation t, bool isOneWay, bool station) : from(f), to(t), isStation(station), isLocked(false), isOneWay(isOneWay) {}

Railway::Railway(SaveData data) {
	using namespace SaveKeys;
	this->from = data.getIsoRotation(FROM);
	this->to = data.getIsoRotation(TO);
	this->isStation = data.getBool(IS_STATION);
	this->isLocked = false;
	this->isOneWay = data.getBool(IS_ONE_WAY);
}
SaveData Railway::getSaveData() {
	using namespace SaveKeys;
	SaveData data(RAILWAY);
	data.addIsoRotation(FROM, this->from);
	data.addIsoRotation(TO, this->to);
	data.addBool(IS_STATION, this->isStation);
	data.addBool(IS_ONE_WAY, this->isOneWay);
	return data;
}

IsoRotation Railway::getFrom() { return this->from; };
IsoRotation Railway::getTo() { return this->to; }
bool Railway::getIsOneWay() { return this->isOneWay; }

bool Railway::operator==(Railway other) { return this->to == other.to && this->from == other.from; }
bool Railway::getIsLocked() { return this->isLocked; };
void Railway::getLock() {
#ifdef _DEBUG
	if (this->isLocked) {
		throw std::runtime_error("Cannot get lock on railway that already has lock");
	}
#endif
	this->isLocked = true;
}
void Railway::releaseLock() {
#ifdef _DEBUG
	if (!this->isLocked) {
		throw std::runtime_error("Cannot release non-existant lock!");
	}
#endif
	this->isLocked = false;
}