#include "Railway.h"
#include "Constants.h"
#include "System/SaveData/SaveData.h"
#include <algorithm>

Railway::Railway(IsoRotation f, IsoRotation t, bool station) : from(f), to(t), isStation(station) {}

Railway::Railway(SaveData data) {
	using namespace SaveKeys;
	this->from = data.getIsoRotation(FROM);
	this->to = data.getIsoRotation(TO);
	this->isStation = data.getBool(IS_STATION);
}
SaveData Railway::getSaveData() {
	using namespace SaveKeys;
	SaveData data(RAILWAY);
	data.addIsoRotation(FROM, this->from);
	data.addIsoRotation(TO, this->to);
	data.addBool(IS_STATION, this->isStation);
	return data;
}

IsoRotation Railway::getFrom() { return this->from; };
IsoRotation Railway::getTo() { return this->to; }