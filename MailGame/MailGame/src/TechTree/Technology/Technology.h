#pragma once
#include <string>
#include <map>

// Default is the technology the player starts off with
#define TECHNOLOGIES \
	f(Default) \
	f(CargoTrucks) \
	f(FastCargoTrucks) \
	f(Trains) \
	f(FastTrains) \
	f(Airplanes) \
	f(FastAirplanes)

/*
 * The different technologies available for the player to unlock
 */
#define f(x) x,\

enum class Technology {
	TECHNOLOGIES
};

#undef f
#define f(x) { Technology::x, #x },\

const std::map<Technology, std::string> TECH_STRS = {
	TECHNOLOGIES
};

#undef f
#undef TECHNOLOGIES

std::string technologyToString(Technology t);
Technology stringToTechnology(std::string s);
